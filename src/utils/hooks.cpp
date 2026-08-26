#include "hooks.h"
#include <MinHook.h>
#include "il2cpp.h"
#include "../features/visual/visual.h"
#include "../features/weaponmod/weaponmod.h"
#include "../features/playermod/playermod.h"
#include "../features/currency/currency.h"
#include "../utils/config.h"
#include "../ui/menu.h"
#include <string>

namespace Hooks
{
  fn_ApplyDamage oApplyDamage = nullptr;

  // ---- Lottery: Drop Count Hook ----
  fn_LotteryDropCount oLotteryDropCount = nullptr;

  int32_t hkLotteryDropCount(void* arg)
  {
    // Safety: validate arg before calling original
    if (!arg || IsBadReadPtr(arg, 0x10)) {
      if (oLotteryDropCount)
        return oLotteryDropCount(arg);
      return 0;
    }

    return CurrencyMod::OnLotteryDropCount(oLotteryDropCount(arg));
  }

  // ---- Match Reward: ShowResult Coroutine Hook ----
  fn_ShowResultCoroutine oShowResultCoroutine = nullptr;

  void* hkShowResultCoroutine(
    void* thisPtr,
    void* winner,
    void* ratingChange,
    bool  showAward,
    int   addExp,
    int   blueTotal,
    bool  firstPlace,
    bool  deadheatDuel,
    bool  param8,
    bool  iAmWinnerInTeam,
    int   addCoin,
    int   addEventCurrency,
    int   winnerCommand,
    int   bpCurrency,
    int   pixelPassCurrency,
    bool  param15,
    int   pixelPassExp,
    int   param17,
    int   param18,
    int   param19,
    bool  clanCurrencyLimitReached,
    void* addDetails,
    void* addModuleChest,
    int   winterPoints,
    void* vipRewards,
    int   springPt1Points,
    int   springPt2Points,
    int   springPt2Currency,
    int   gemsByHarvester,
    int   balanceBrawlPoints,
    int   addGems,
    void* templateEventItems
  )
  {
    if (CurrencyMod::Settings::bEnableRewardMultiplier) {
      // Multiply only coins as others are server-sided
      addCoin = static_cast<int>(addCoin * CurrencyMod::Settings::fCoinsMultiplier);
    }

    return oShowResultCoroutine(
      thisPtr, winner, ratingChange, showAward, addExp, blueTotal, firstPlace, deadheatDuel, param8, iAmWinnerInTeam,
      addCoin, addEventCurrency, winnerCommand, bpCurrency, pixelPassCurrency, param15, pixelPassExp, param17, param18,
      param19, clanCurrencyLimitReached, addDetails, addModuleChest, winterPoints, vipRewards, springPt1Points,
      springPt2Points, springPt2Currency, gemsByHarvester, balanceBrawlPoints, addGems, templateEventItems
    );
  }

  // ---- ApplyDamage Hook ----
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
    // When Player_move_c::Update is running, we're in-match — lottery hooks should be disabled
    CurrencyMod::Settings::bSafeToModify = false;

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

    // Re-enable lottery modifications after the update completes
    // (we're still in-match but the critical section has passed)
    CurrencyMod::Settings::bSafeToModify = true;
  }

  fn_CBD_Trigger oCBD_Trigger = nullptr;
  void           hkCBD_Trigger() { /* NOP */ }

  fn_CBD_Show oCBD_Show = nullptr;
  void        hkCBD_Show() { /* NOP */ }


  // ---- PixelPass Premium: HasPremium ----
  fn_PPBoolGetter oPPHasPremium1 = nullptr;
  fn_PPBoolGetter oPPHasPremium2 = nullptr;
  fn_PPBoolGetter oPPHasPremium3 = nullptr;
  fn_PPBoolGetter oPPHasPremium4 = nullptr;
  fn_PPBoolGetter oPPHasPremium5 = nullptr;

  bool hkPPHasPremium1(void* thisPtr)
  {
    if (CurrencyMod::Settings::bSpoofPixelPassPremium)
      return true;
    return oPPHasPremium1(thisPtr);
  }

  bool hkPPHasPremium2(void* thisPtr)
  {
    if (CurrencyMod::Settings::bSpoofPixelPassPremium)
      return true;
    return oPPHasPremium2(thisPtr);
  }

  bool hkPPHasPremium3(void* thisPtr)
  {
    if (CurrencyMod::Settings::bSpoofPixelPassPremium)
      return true;
    return oPPHasPremium3(thisPtr);
  }

  bool hkPPHasPremium4(void* thisPtr)
  {
    if (CurrencyMod::Settings::bSpoofPixelPassPremium)
      return true;
    return oPPHasPremium4(thisPtr);
  }

  bool hkPPHasPremium5(void* thisPtr)
  {
    if (CurrencyMod::Settings::bSpoofPixelPassPremium)
      return true;
    return oPPHasPremium5(thisPtr);
  }

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

    // 3. Hook the drop count getter
    auto lotteryDropCountAddr = reinterpret_cast<void*>(gaBase + Offsets::Lottery::LotteryDropCount_RVA);
    MH_CreateHook(
      lotteryDropCountAddr, reinterpret_cast<void*>(&hkLotteryDropCount), reinterpret_cast<void**>(&oLotteryDropCount)
    );

    // 5. Match Reward: Hook ShowResult coroutine
    auto showResultAddr = reinterpret_cast<void*>(gaBase + Offsets::MatchReward::ShowResultCoroutine_RVA);
    MH_CreateHook(
      showResultAddr, reinterpret_cast<void*>(&hkShowResultCoroutine), reinterpret_cast<void**>(&oShowResultCoroutine)
    );

    // Dynamic resolution for Player_move_c::Update
    void* updateMethod = IL2CPP::class_get_method_from_name((void*) Offsets::Classes::PlayerMoveC, "Update", 0);
    if (updateMethod) {
      void* updatePointer = *reinterpret_cast<void**>(updateMethod);
      MH_CreateHook(
        updatePointer, reinterpret_cast<void*>(&hkPlayerMoveC_Update), reinterpret_cast<void**>(&oPlayerMoveC_Update)
      );
    }

    // 6. PixelPass Premium: Hook multiple bool getters on the controller
    auto pp1Addr = reinterpret_cast<void*>(gaBase + Offsets::PixelPass::HasPremium_RVA);
    MH_CreateHook(pp1Addr, reinterpret_cast<void*>(&hkPPHasPremium1), reinterpret_cast<void**>(&oPPHasPremium1));

    auto pp2Addr = reinterpret_cast<void*>(gaBase + Offsets::PixelPass::HasPremium2_RVA);
    MH_CreateHook(pp2Addr, reinterpret_cast<void*>(&hkPPHasPremium2), reinterpret_cast<void**>(&oPPHasPremium2));

    auto pp3Addr = reinterpret_cast<void*>(gaBase + Offsets::PixelPass::HasPremium3_RVA);
    MH_CreateHook(pp3Addr, reinterpret_cast<void*>(&hkPPHasPremium3), reinterpret_cast<void**>(&oPPHasPremium3));

    auto pp4Addr = reinterpret_cast<void*>(gaBase + Offsets::PixelPass::HasPremium4_RVA);
    MH_CreateHook(pp4Addr, reinterpret_cast<void*>(&hkPPHasPremium4), reinterpret_cast<void**>(&oPPHasPremium4));

    auto pp5Addr = reinterpret_cast<void*>(gaBase + Offsets::PixelPass::HasPremium5_RVA);
    MH_CreateHook(pp5Addr, reinterpret_cast<void*>(&hkPPHasPremium5), reinterpret_cast<void**>(&oPPHasPremium5));

    MH_EnableHook(MH_ALL_HOOKS);

    return true;
  }

  void SetupMenu()
  {
    Menu::AddMenuItem({"-- SETTINGS --", Menu::ItemType::Header});
    Menu::AddMenuItem({"Bypass Anti-Cheat", Menu::ItemType::Bool, &Settings::bAntiCheatBypass});
    Menu::AddMenuItem(
      {"Save Config", Menu::ItemType::Action, nullptr, nullptr, nullptr, 0.0f, 0.0f, 0.0f, 0, 0, 0,
       []() { Config::Save(); }}
    );
    Menu::AddMenuItem(
      {"Load Config", Menu::ItemType::Action, nullptr, nullptr, nullptr, 0.0f, 0.0f, 0.0f, 0, 0, 0,
       []() { Config::Load(); }}
    );
  }

  void Shutdown()
  {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
  }
}  // namespace Hooks
