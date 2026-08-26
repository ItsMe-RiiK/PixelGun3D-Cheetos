#pragma once

namespace CurrencyMod
{
  namespace Settings
  {
    // Lottery
    inline bool bEnableLotteryModifier = false;
    inline bool bModifyChestOutput     = false;
    inline int  iChestOutputAmount     = 99999;
    inline bool bSafeToModify          = false;  // Safety flag prevent crash while match

    // Match Reward
    inline bool  bEnableRewardMultiplier = false;
    inline float fCoinsMultiplier        = 99999.0f;

    // PixelPass
    inline bool bSpoofPixelPassPremium = false;
  }  // namespace Settings

  void InitMenu();

  // Logic functions
  int OnLotteryDropCount(int originalCount);
}  // namespace CurrencyMod
