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
        IL2CPP::WriteField(ws, isUnlimitedAmmo, true);
        int maxAmmoVal = IL2CPP::ReadField<int>(ws, maxAmmo);
        if (maxAmmoVal > 0) {
          IL2CPP::WriteField(ws, ammoInClip, maxAmmoVal);
        }
      }

      if (Features::bNoReload) {
        int maxAmmoVal  = IL2CPP::ReadField<int>(ws, maxAmmo);
        int currentAmmo = IL2CPP::ReadField<int>(ws, ammoInClip);
        if (maxAmmoVal > 0 && currentAmmo <= 0) {
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
          float originalRadius = IL2CPP::ReadField<float>(ws, radiusRoundMelee);
          float boostedRadius  = originalRadius * Features::fReachMultiplier;
          if (boostedRadius < 5.0f)
            boostedRadius = 5.0f * Features::fReachMultiplier;

          IL2CPP::WriteField(ws, radiusRoundMelee, boostedRadius);
          IL2CPP::WriteField(ws, distancePiercingMelee, boostedRadius * 2.0f);
        }
      }
    } catch (...) {
      // Prevent crash
    }
  }
}  // namespace WeaponMod
