#include "lottery.h"
#include "../../utils/il2cpp.h"

namespace LotteryMod
{
  bool bEnableLotteryModifier = false;
  int  iChestPrice            = 0;
  bool bModifyChestOutput     = false;
  int  iChestOutputAmount     = 250;

  int OnFreeLottery(int originalPrice)
  {
    if (bEnableLotteryModifier) {
      // Prevent matchmaking crash by ignoring lottery modifications during a match
      if (IL2CPP::GetLocalPlayerMoveC() != nullptr) {
        return originalPrice;
      }
      return iChestPrice;
    }
    return originalPrice;
  }

  int OnLotteryDropCount(int originalCount)
  {
    if (bEnableLotteryModifier && bModifyChestOutput) {
      // Prevent matchmaking crash by ignoring lottery modifications during a match
      if (IL2CPP::GetLocalPlayerMoveC() != nullptr) {
        return originalCount;
      }
      return iChestOutputAmount;
    }
    return originalCount;
  }
}  // namespace LotteryMod
