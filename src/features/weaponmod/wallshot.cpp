#include "weaponmod.h"
#include "../../utils/il2cpp.h"

namespace WeaponMod {
  void ApplyWallshot(void* ws)
  {
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;

    static size_t wallBreakingDamageMultiplierOffset = 0;
    static size_t isWallBreakingOffset               = 0;
    static size_t breakoutOffset                     = 0;
    static size_t superBreakoutOffset                = 0;
    static bool   fetchedWallshotOffsets             = false;

    if (!fetchedWallshotOffsets) {
      void* wsClass = (void*) Offsets::Classes::WeaponSounds;

      isWallBreakingOffset = IL2CPP::GetFieldOffset(wsClass, "isWallBraking");
      if (!isWallBreakingOffset)
        isWallBreakingOffset = IL2CPP::GetFieldOffset(wsClass, "wallBraking");

      wallBreakingDamageMultiplierOffset =
        IL2CPP::GetFieldOffset(wsClass, "wallBrakingDamageMultiplier");
      if (!wallBreakingDamageMultiplierOffset)
        wallBreakingDamageMultiplierOffset =
          IL2CPP::GetFieldOffset(wsClass, "damageWallBrakingMultiplier");
      if (!wallBreakingDamageMultiplierOffset)
        wallBreakingDamageMultiplierOffset =
          IL2CPP::GetFieldOffset(wsClass, "damageMultiplierThroughWall");
      if (!wallBreakingDamageMultiplierOffset)
        wallBreakingDamageMultiplierOffset =
          IL2CPP::GetFieldOffset(wsClass, "damageWallMultiplier");

      breakoutOffset      = IL2CPP::GetFieldOffset(wsClass, "bulletBreakout");
      superBreakoutOffset = IL2CPP::GetFieldOffset(wsClass, "bulletSuperBreakout");

      fetchedWallshotOffsets = true;
    }

    // FORCE disable breakout to ensure bullets hit players instead of ignoring all colliders
    if (breakoutOffset > 0)
      IL2CPP::WriteField(ws, breakoutOffset, false);
    if (superBreakoutOffset > 0)
      IL2CPP::WriteField(ws, superBreakoutOffset, false);

    if (isWallBreakingOffset > 0)
      IL2CPP::WriteField(ws, isWallBreakingOffset, true);
    if (wallBreakingDamageMultiplierOffset > 0)
      IL2CPP::WriteField(ws, wallBreakingDamageMultiplierOffset, 9999.0f);

    // Also enable piercing for melee
    IL2CPP::WriteField(ws, Offsets::WeaponSounds::isPiercingMelee, true);
    IL2CPP::WriteField(ws, Offsets::WeaponSounds::distancePiercingMelee, 9999.0f);
  }
}  // namespace WeaponMod
