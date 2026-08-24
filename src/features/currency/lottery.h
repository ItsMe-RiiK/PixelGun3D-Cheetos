#pragma once
#include <cstdint>

namespace LotteryMod
{
  extern bool bEnableLotteryModifier;
  extern int  iChestPrice;
  extern bool bModifyChestOutput;
  extern int  iChestOutputAmount;

  int OnFreeLottery(int originalPrice);
  int OnLotteryDropCount(int originalCount);
}  // namespace LotteryMod
