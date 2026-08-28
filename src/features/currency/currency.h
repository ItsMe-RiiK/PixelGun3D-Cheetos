#pragma once

namespace CurrencyMod
{
  namespace Settings
  {
    // Lottery
    inline bool bModifyChestOutput = false;
    inline int  iChestOutputAmount = 250;

    // Free Store
    inline bool bFreeStore = false;

    // Match Reward
    inline bool  bEnableRewardMultiplier = false;
    inline float fCoinsMultiplier        = 1.0f;

    // PixelPass
    inline bool bSpoofPixelPassPremium = false;
  }  // namespace Settings

  void InitMenu();
  int  OnLotteryDropCount(int originalCount);
}  // namespace CurrencyMod
