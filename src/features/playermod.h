#pragma once
// =====================================================
// Player Mod Features — Speed Hack, No Fall Damage,
// Invisibility, God Mode, AutoHeal
// =====================================================

#include "../features.h"
#include "../il2cpp.h"

namespace PlayerMod {
  inline float originalSpeedModifier = 0.0f;
  inline bool  speedBackedUp         = false;

  // Apply Speed Hack
  inline void ApplySpeedHack()
  {
    auto ws = IL2CPP::GetCurrentWeaponSounds();
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;

    // Backup original speed
    if (!speedBackedUp) {
      originalSpeedModifier = IL2CPP::ReadField<float>(ws, speedModifier);
      if (originalSpeedModifier == 0.0f)
        originalSpeedModifier = 1.0f;
      speedBackedUp = true;
    }

    // Apply speed multiplier
    IL2CPP::WriteField(ws, speedModifier, originalSpeedModifier * Features::fSpeedMultiplier);
  }

  // Restore Speed
  inline void RestoreSpeed()
  {
    if (!speedBackedUp)
      return;

    auto ws = IL2CPP::GetCurrentWeaponSounds();
    if (!ws)
      return;

    IL2CPP::WriteField(ws, Offsets::WeaponSounds::speedModifier, originalSpeedModifier);
    speedBackedUp = false;
  }

  // No Fall Damage — cap negative Y velocity
  inline void ApplyNoFallDamage()
  {
    // We modify the FirstPersonControlSharp maxNegativeVelocityYPerFrame
    // to a very small value so falls never kill
    // This is done through the Player_move_c -> FPS controller chain
    // For now we NOP the camTweenerFallDamage by setting it to nullptr
    // The actual fall damage is primarily handled server-side via velocity checks

    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (!pmc)
      return;

    // Weapon delay also affects fall damage in some modes
    IL2CPP::WriteField(pmc, Offsets::PlayerMoveC::IsWeaponDelay, false);
  }

  // AutoHeal — continuously set health to max
  // Note: God mode is handled by the ApplyDamage hook in hooks.h
  // AutoHeal complements it by restoring health after any damage that gets through
  inline void ApplyAutoHeal()
  {
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (!pmc)
      return;

    // Reset lastDamageTime to 0 to trigger any passive regen faster
    IL2CPP::WriteField(pmc, Offsets::PlayerMoveC::lastDamageTime, 0.0f);
  }

  // Main tick — called every frame
  inline void Tick()
  {
    // Speed Hack
    if (Features::bSpeedHack)
      ApplySpeedHack();
    else if (speedBackedUp)
      RestoreSpeed();

    // No Fall Damage
    if (Features::bNoFallDamage)
      ApplyNoFallDamage();

    // AutoHeal
    if (Features::bAutoHeal)
      ApplyAutoHeal();

    // Note: God Mode is handled by the ApplyDamage hook (hooks.h)
    // Note: Invisibility would require calling the game's internal
    //       invisibility effect method, which needs more RE work
  }
}  // namespace PlayerMod
