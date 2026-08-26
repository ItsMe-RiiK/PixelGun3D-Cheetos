#include "currency.h"
#include "../../utils/il2cpp.h"
#include "../../utils/offsets.h"

namespace CurrencyMod
{
  int OnLotteryDropCount(int originalCount)
  {
    if (!Settings::bEnableLotteryModifier || !Settings::bModifyChestOutput || !Settings::bSafeToModify)
      return originalCount;

    return Settings::iChestOutputAmount;
  }
}  // namespace CurrencyMod
