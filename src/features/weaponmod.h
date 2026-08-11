#pragma once
// =====================================================
// Weapon Mod Features — Infinite Ammo, No Reload,
// 100% Crit, Extended Melee Reach
// =====================================================

#include "../features.h"
#include "../il2cpp.h"

namespace WeaponMod {
  // Main tick — called every frame
  inline void Tick()
  {
    auto ws = IL2CPP::GetCurrentWeaponSounds();
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;

    // Infinite Ammo
    if (Features::bInfiniteAmmo) {
      IL2CPP::WriteField(ws, isUnlimitedAmmo, true);
    }

    // No Reload — keep ammo maxed
    if (Features::bNoReload) {
      int maxAmmoVal = IL2CPP::ReadField<int>(ws, maxAmmo);
      if (maxAmmoVal > 0) {
        IL2CPP::WriteField(ws, ammoInClip, maxAmmoVal);
      }
    }

    // 100% Crit Chance
    if (Features::bCritChance100) {
      IL2CPP::WriteField(ws, criticalHitChance, 100);
      IL2CPP::WriteField(ws, criticalHitCoef, Features::fCritMultiplier);
    }

    // Extended Melee Reach
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
  }
}  // namespace WeaponMod
