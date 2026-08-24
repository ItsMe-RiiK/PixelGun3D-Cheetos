#include "hooks.h"
#include <MinHook.h>
#include "il2cpp.h"
#include "../features/visual/visual.h"
#include "../features/weaponmod/weaponmod.h"
#include "../features/playermod/playermod.h"
#include "../features/currency/lottery.h"

namespace Hooks
{
  fn_ApplyDamage oApplyDamage = nullptr;
  fn_FreeLottery oFreeLottery = nullptr;

  int32_t hkFreeLottery(void* arg)
  {
    int32_t overrideValue = LotteryMod::OnFreeLottery(oFreeLottery(arg));
    return overrideValue;
  }

  fn_LotteryDropCount oLotteryDropCount = nullptr;

  int32_t hkLotteryDropCount(void* arg)
  {
    int32_t overrideValue = LotteryMod::OnLotteryDropCount(oLotteryDropCount(arg));
    return overrideValue;
  }

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
    if (PlayerMod::OnApplyDamage(thisPtr)) {
      return;
    }

    WeaponMod::ApplyAutoHeadshot(thisPtr, damageType);

    oApplyDamage(
      thisPtr, damage, attacker, collider, hitPoint, damageType, typeDead, weaponId, someInt, someEnum, methodInfo
    );
  }

  using fn_OnEventFired         = bool (*)(void* thisPtr, uint8_t eventCode, void* eventData, void* methodInfo);
  fn_OnEventFired oOnEventFired = nullptr;

  bool hkOnEventFired(void* thisPtr, uint8_t eventCode, void* eventData, void* methodInfo)
  {
    if (PlayerMod::OnEventFired(thisPtr, eventCode)) {
      return true;
    }
    return oOnEventFired(thisPtr, eventCode, eventData, methodInfo);
  }

  using fn_PlayerMoveC_Update               = void (*)(void* thisPtr);
  fn_PlayerMoveC_Update oPlayerMoveC_Update = nullptr;

  void hkPlayerMoveC_Update(void* thisPtr)
  {
    // Apply our mods BEFORE the game's Update so our values are ready when the game processes shooting
    bool isLocalPlayer = false;
    if (thisPtr) {
      Offsets::PlayerMoveC::InitDynamicOffsets();
      Offsets::SkinName::InitDynamicOffsets();

      if (Offsets::PlayerMoveC::mySkinNameOffset > 0 && Offsets::SkinName::isMineOffset > 0) {
        void* skinName = IL2CPP::SafeReadField<void*>(thisPtr, Offsets::PlayerMoveC::mySkinNameOffset);
        if (skinName) {
          isLocalPlayer = IL2CPP::SafeReadField<bool>(skinName, Offsets::SkinName::isMineOffset);
        }
      }
      else {
        isLocalPlayer = (thisPtr == IL2CPP::GetLocalPlayerMoveC());
      }
    }

    if (isLocalPlayer) {
      Visual::TickMainThread();
      WeaponMod::Tick();
      PlayerMod::Tick();
    }

    // Run the game's original Update
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

    // 1. Hook ApplyDamage for no fall damage
    void* pApplyDamage = reinterpret_cast<void*>(gaBase + Offsets::PlayerDamageable::ApplyDamage_RVA);
    if (MH_CreateHook(pApplyDamage, (void*) &hkApplyDamage, reinterpret_cast<void**>(&oApplyDamage)) != MH_OK) {
      return false;
    }
    MH_EnableHook(pApplyDamage);

    // 2. Hook OnEventFired to block network damage RPCs for God Mode
    void* pOnEventFired = reinterpret_cast<void*>(gaBase + Offsets::PlayerMoveC::OnEventFired_RVA);
    if (MH_CreateHook(pOnEventFired, (void*) &hkOnEventFired, reinterpret_cast<void**>(&oOnEventFired)) != MH_OK) {
      return false;
    }
    MH_EnableHook(pOnEventFired);


    auto cbdTriggerAddr = reinterpret_cast<void*>(gaBase + Offsets::AntiCheat::CBD_Trigger_RVA);
    MH_CreateHook(cbdTriggerAddr, reinterpret_cast<void*>(&hkCBD_Trigger), reinterpret_cast<void**>(&oCBD_Trigger));

    auto cbdShowAddr = reinterpret_cast<void*>(gaBase + Offsets::AntiCheat::CBD_ShowBanner_RVA);
    MH_CreateHook(cbdShowAddr, reinterpret_cast<void*>(&hkCBD_Show), reinterpret_cast<void**>(&oCBD_Show));

    auto freeLotteryAddr = reinterpret_cast<void*>(gaBase + Offsets::Lottery::PriceModifier_RVA);
    MH_CreateHook(freeLotteryAddr, reinterpret_cast<void*>(&hkFreeLottery), reinterpret_cast<void**>(&oFreeLottery));

    auto lotteryDropCountAddr = reinterpret_cast<void*>(gaBase + Offsets::Lottery::LotteryDropCount_RVA);
    MH_CreateHook(
      lotteryDropCountAddr, reinterpret_cast<void*>(&hkLotteryDropCount), reinterpret_cast<void**>(&oLotteryDropCount)
    );

    // Dynamic resolution for Player_move_c::Update
    void* updateMethod = IL2CPP::class_get_method_from_name((void*) Offsets::Classes::PlayerMoveC, "Update", 0);
    if (updateMethod) {
      void* updatePointer = *reinterpret_cast<void**>(updateMethod);
      MH_CreateHook(
        updatePointer, reinterpret_cast<void*>(&hkPlayerMoveC_Update), reinterpret_cast<void**>(&oPlayerMoveC_Update)
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
