#include "weaponmod.h"
#include "../../utils/il2cpp.h"

namespace WeaponMod
{
  void ApplyCritChance(void* ws)
  {
    if (!ws)
      return;
    using namespace Offsets::WeaponSounds;
    IL2CPP::WriteField(ws, criticalHitChance, 100);
    IL2CPP::WriteField(ws, criticalHitCoef, Settings::fCritMultiplier);
  }
}  // namespace WeaponMod
