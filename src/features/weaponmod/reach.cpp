#include "weaponmod.h"
#include "../../utils/il2cpp.h"

namespace WeaponMod {
  void ApplyReach(void* ws)
  {
    if (!ws)
      return;
    using namespace Offsets::WeaponSounds;

    bool isMeleeWeapon = IL2CPP::ReadField<bool>(ws, isMelee);
    if (isMeleeWeapon) {
      // Fixed 10x boost to avoid infinite multiplier stacking
      IL2CPP::WriteField(ws, radiusRoundMelee, 50.0f);
      IL2CPP::WriteField(ws, distancePiercingMelee, 50.0f);
    }
    else {
      // Guns
      IL2CPP::WriteField(ws, 0x594, 9999.0f);  // lengthShootFromPrefab
      IL2CPP::WriteField(ws, 0x4CC, 9999.0f);  // shootDistanceIfZoom
    }
  }
}  // namespace WeaponMod
