#include "weaponmod.h"
#include "../../utils/il2cpp.h"

namespace WeaponMod {
  void ApplyWallshot(void* ws)
  {
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;

    Offsets::WeaponSounds::InitDynamicOffsets();

    // FORCE disable breakout to ensure bullets hit players instead of ignoring all colliders
    IL2CPP::WriteFieldIfValid(ws, breakoutOffset, false);
    IL2CPP::WriteFieldIfValid(ws, superBreakoutOffset, false);

    IL2CPP::WriteFieldIfValid(ws, isWallBreakingOffset, true);
    IL2CPP::WriteFieldIfValid(ws, wallBreakingDamageMultiplierOffset, 9999.0f);

    // Also enable piercing for melee
    IL2CPP::WriteField(ws, Offsets::WeaponSounds::isPiercingMelee, true);
    IL2CPP::WriteField(ws, Offsets::WeaponSounds::distancePiercingMelee, 9999.0f);
  }
}  // namespace WeaponMod
