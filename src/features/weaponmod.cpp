#include "weaponmod.h"
#include "../utils/settings.h"
#include "../utils/il2cpp.h"

namespace WeaponMod {
  void Tick()
  {
    try {
      auto ws = IL2CPP::GetCurrentWeaponSounds();
      if (!ws)
        return;

      using namespace Offsets::WeaponSounds;

      if (Features::bInfiniteAmmo) {
        // Spare ammo infinite
        IL2CPP::WriteField(ws, isUnlimitedAmmo, true);

        // Main clip infinite (No Reload logic)
        IL2CPP::WriteField(ws, 0x30C, true);  // IsInstantReload
        IL2CPP::WriteField(ws, 0x310, 0);     // _numShotsForInstantReload

        int maxAmmoVal = IL2CPP::ReadField<int>(ws, maxAmmo);
        if (maxAmmoVal > 0) {
          IL2CPP::WriteField(ws, ammoInClip, maxAmmoVal);
        }
      }

      if (Features::bCritChance100) {
        IL2CPP::WriteField(ws, criticalHitChance, 100);
        IL2CPP::WriteField(ws, criticalHitCoef, Features::fCritMultiplier);
      }

      if (Features::bReach) {
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
    } catch (...) {
      // Prevent crash
    }
  }
}  // namespace WeaponMod
