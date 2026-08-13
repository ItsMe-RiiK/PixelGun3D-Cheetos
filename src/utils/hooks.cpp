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
    void*       someEnum
  )
  {
    if (Features::bGodMode) {
      auto localPMC = IL2CPP::GetLocalPlayerMoveC();
      if (localPMC && thisPtr) {
        auto thisPMC = IL2CPP::ReadField<void*>(thisPtr, Offsets::PlayerDamageable::playerMoveC);
        if (thisPMC == localPMC) {
          return;
        }
      }
    }

    oApplyDamage(
      thisPtr, damage, attacker, collider, hitPoint, damageType, typeDead, weaponId, someInt,
      someEnum
    );
  }

  using fn_PlayerMoveC_Update               = void (*)(void* thisPtr);
  fn_PlayerMoveC_Update oPlayerMoveC_Update = nullptr;

  void hkPlayerMoveC_Update(void* thisPtr)
  {
    if (thisPtr == IL2CPP::GetLocalPlayerMoveC()) {
      if (Features::bPlayerESP || Features::bSkeletonESP) {
        Visual::TickMainThread();
      }
      Combat::Tick();
      WeaponMod::Tick();
      PlayerMod::Tick();
    }
    if (oPlayerMoveC_Update) {
      oPlayerMoveC_Update(thisPtr);
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

    auto applyDamageAddr =
      reinterpret_cast<void*>(gaBase + Offsets::PlayerDamageable::ApplyDamage_RVA);
    MH_CreateHook(
      applyDamageAddr, reinterpret_cast<void*>(&hkApplyDamage),
      reinterpret_cast<void**>(&oApplyDamage)
    );

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
