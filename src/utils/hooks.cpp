#include "hooks.h"
#include <MinHook.h>
#include "../utils/settings.h"
#include "il2cpp.h"
#include "../features/visual.h"
#include "../features/combat.h"
#include "../features/weaponmod.h"
#include "../features/playermod.h"

namespace Hooks {
  fn_ApplyDamage oApplyDamage = nullptr;

  void hkApplyDamage(
    void*       thisPtr,
    float       damage,
    void*       attacker,
    void*       collider,
    void*       hitPoint,
    int         damageType,
    int         typeDead,
    const char* weaponId,
    int         someInt,
    void*       someEnum,
    void*       methodInfo
  )
  {
    if (Features::bGodMode) {
      auto localPMC = IL2CPP::GetLocalPlayerMoveC();
      if (localPMC && thisPtr) {
        auto thisPMC = IL2CPP::ReadField<void*>(thisPtr, Offsets::PlayerDamageable::playerMoveC);
        if (thisPMC == localPMC) {
          if (Features::bGodMode) {
            return;  // God mode blocks all local damage (including fall damage)
          }
        }
      }
    }

    oApplyDamage(
      thisPtr, damage, attacker, collider, hitPoint, damageType, typeDead, weaponId, someInt,
      someEnum, methodInfo
    );
  }

  using fn_OnEventFired =
    bool (*)(void* thisPtr, uint8_t eventCode, void* eventData, void* methodInfo);
  fn_OnEventFired oOnEventFired = nullptr;

  bool hkOnEventFired(void* thisPtr, uint8_t eventCode, void* eventData, void* methodInfo)
  {
    if (Features::bGodMode) {
      if (thisPtr != nullptr && thisPtr == IL2CPP::GetLocalPlayerMoveC()) {
        // Block network damage RPCs
        // 10 = ApplyDamageRPC, 11 = ApplyDebuffRPC, 24 = GetDamageRPC, 47 = HitByVehicleRPC
        if (eventCode == 10 || eventCode == 11 || eventCode == 24 || eventCode == 47) {
          return true;  // Ignore the event
        }
      }
    }
    return oOnEventFired(thisPtr, eventCode, eventData, methodInfo);
  }

  fn_MoveSpeedMultiplier oMoveSpeedMultiplier = nullptr;
  float                  hkMoveSpeedMultiplier(void* thisPtr, void* methodInfo)
  {
    float baseSpeed = oMoveSpeedMultiplier(thisPtr, methodInfo);
    if (Features::bSpeedHack) {
      return baseSpeed * Features::fSpeedMultiplier;
    }
    return baseSpeed;
  }

  using fn_PlayerMoveC_Update               = void (*)(void* thisPtr);
  fn_PlayerMoveC_Update oPlayerMoveC_Update = nullptr;

  void hkPlayerMoveC_Update(void* thisPtr)
  {
    // Run the game's original Update first
    if (oPlayerMoveC_Update) {
      oPlayerMoveC_Update(thisPtr);
    }

    // Apply our mods AFTER the game's Update so our values don't get overwritten
    if (thisPtr == IL2CPP::GetLocalPlayerMoveC()) {
      if (Features::bPlayerESP || Features::bSkeletonESP) {
        Visual::TickMainThread();
      }
      Combat::Tick();
      WeaponMod::Tick();
      PlayerMod::Tick();
    }
  }

  fn_CBD_Trigger oCBD_Trigger = nullptr;
  void           hkCBD_Trigger() { /* NOP */ }

  fn_CBD_Show oCBD_Show = nullptr;
  void        hkCBD_Show() { /* NOP */ }

  fn_Present oPresent = nullptr;

  bool Init()
  {
    if (MH_Initialize() != MH_OK)
      return false;

    auto gaBase = IL2CPP::gameAssemblyBase;

    // 1. Hook ApplyDamage for no fall damage
    void* pApplyDamage =
      reinterpret_cast<void*>(gaBase + Offsets::PlayerDamageable::ApplyDamage_RVA);
    if (
      MH_CreateHook(pApplyDamage, (void*) &hkApplyDamage, reinterpret_cast<void**>(&oApplyDamage))
      != MH_OK
    ) {
      return false;
    }
    MH_EnableHook(pApplyDamage);

    // 2. Hook OnEventFired to block network damage RPCs for God Mode
    void* pOnEventFired = reinterpret_cast<void*>(gaBase + 0x157D550);  // RVA from dump.cs
    if (
      MH_CreateHook(
        pOnEventFired, (void*) &hkOnEventFired, reinterpret_cast<void**>(&oOnEventFired)
      )
      != MH_OK
    ) {
      return false;
    }
    MH_EnableHook(pOnEventFired);

    // 3. Hook get_MoveSpeedMultiplier for Speed Hack
    void* pMoveSpeed = reinterpret_cast<void*>(gaBase + 0x9A3ED0);  // RVA from dump.cs
    if (
      MH_CreateHook(
        pMoveSpeed, (void*) &hkMoveSpeedMultiplier, reinterpret_cast<void**>(&oMoveSpeedMultiplier)
      )
      != MH_OK
    ) {
      return false;
    }
    MH_EnableHook(pMoveSpeed);

    auto cbdTriggerAddr = reinterpret_cast<void*>(gaBase + Offsets::AntiCheat::CBD_Trigger_RVA);
    MH_CreateHook(
      cbdTriggerAddr, reinterpret_cast<void*>(&hkCBD_Trigger),
      reinterpret_cast<void**>(&oCBD_Trigger)
    );

    auto cbdShowAddr = reinterpret_cast<void*>(gaBase + Offsets::AntiCheat::CBD_ShowBanner_RVA);
    MH_CreateHook(
      cbdShowAddr, reinterpret_cast<void*>(&hkCBD_Show), reinterpret_cast<void**>(&oCBD_Show)
    );

    // Dynamic resolution for Player_move_c::Update
    void* updateMethod =
      IL2CPP::class_get_method_from_name((void*) Offsets::Classes::PlayerMoveC, "Update", 0);
    if (updateMethod) {
      void* updatePointer = *reinterpret_cast<void**>(updateMethod);
      MH_CreateHook(
        updatePointer, reinterpret_cast<void*>(&hkPlayerMoveC_Update),
        reinterpret_cast<void**>(&oPlayerMoveC_Update)
      );
    }

    MH_EnableHook(MH_ALL_HOOKS);

    return true;
  }

  void Shutdown()
  {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
  }
}  // namespace Hooks
