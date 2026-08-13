#include "playermod.h"
#include "../utils/settings.h"
#include "../utils/il2cpp.h"

namespace PlayerMod {
  float originalSpeedModifier = 0.0f;
  bool  speedBackedUp         = false;

  void ApplySpeedHack()
  {
    auto ws = IL2CPP::GetCurrentWeaponSounds();
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;

    if (!speedBackedUp) {
      originalSpeedModifier = IL2CPP::ReadField<float>(ws, speedModifier);
      if (originalSpeedModifier == 0.0f)
        originalSpeedModifier = 1.0f;
      speedBackedUp = true;
    }

    IL2CPP::WriteField(ws, speedModifier, originalSpeedModifier * Features::fSpeedMultiplier);
  }

  void RestoreSpeed()
  {
    if (!speedBackedUp)
      return;

    auto ws = IL2CPP::GetCurrentWeaponSounds();
    if (!ws)
      return;

    IL2CPP::WriteField(ws, Offsets::WeaponSounds::speedModifier, originalSpeedModifier);
    speedBackedUp = false;
  }

  void ApplyNoFallDamage()
  {
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (!pmc)
      return;

    IL2CPP::WriteField(pmc, Offsets::PlayerMoveC::IsWeaponDelay, false);
  }

  void ApplyAutoHeal()
  {
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (!pmc)
      return;

    IL2CPP::WriteField(pmc, Offsets::PlayerMoveC::lastDamageTime, 0.0f);
  }

  void ApplyInvisibility()
  {
    auto ws = IL2CPP::GetCurrentWeaponSounds();
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;
    IL2CPP::WriteField(ws, isInvisibleReload, true);
    IL2CPP::WriteField(ws, isInvisibleAfterKill, true);
    IL2CPP::WriteField(ws, invisibleAfterKillTime, 9999.0f);
  }

  void Tick()
  {
    try {
      if (Features::bSpeedHack)
        ApplySpeedHack();
      else if (speedBackedUp)
        RestoreSpeed();

      if (Features::bNoFallDamage)
        ApplyNoFallDamage();

      if (Features::bAutoHeal)
        ApplyAutoHeal();

      if (Features::bInvisibility)
        ApplyInvisibility();
    } catch (...) {
      // Prevent crash
    }
  }
}  // namespace PlayerMod
