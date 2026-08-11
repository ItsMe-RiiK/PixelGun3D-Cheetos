#pragma once
// =====================================================
// Combat Features — No Recoil, No Spread, Rapid Fire,
// AOE Bullets, Instant Charge
// =====================================================

#include "../features.h"
#include "../il2cpp.h"

namespace Combat {
  // Saved original values for restoration
  struct WeaponSoundsBackup
  {
    float tekKoof;
    float upKoofFire;
    float downKoofFirst;
    float downKoof;
    float moveScatterCoeff;
    bool  firstShotScatter;
    float recoilCoeffZoom;
    float upKoofFireZoom;
    float downKoofFirstZoom;
    float downKoofZoom;
    float moveScatterCoeffZoom;
    bool  firstShotScatterZoom;
    float shootDelay;
    float bulletDelay;
    float delayTimer;
    float chargeTime;
    bool  isSectorsAOE;
    float sectorsAOEAngleFront;
    float sectorsAOERadius;
    bool  hasBackup;
  };

  inline WeaponSoundsBackup backup           = {};
  inline void*              lastWeaponSounds = nullptr;

  // Backup original weapon values when weapon changes
  inline void BackupWeaponSounds(void* ws)
  {
    if (!ws || ws == lastWeaponSounds)
      return;
    lastWeaponSounds = ws;

    using namespace Offsets::WeaponSounds;
    backup.tekKoof              = IL2CPP::ReadField<float>(ws, tekKoof);
    backup.upKoofFire           = IL2CPP::ReadField<float>(ws, upKoofFire);
    backup.downKoofFirst        = IL2CPP::ReadField<float>(ws, downKoofFirst);
    backup.downKoof             = IL2CPP::ReadField<float>(ws, downKoof);
    backup.moveScatterCoeff     = IL2CPP::ReadField<float>(ws, moveScatterCoeff);
    backup.firstShotScatter     = IL2CPP::ReadField<bool>(ws, firstShotScatter);
    backup.recoilCoeffZoom      = IL2CPP::ReadField<float>(ws, recoilCoeffZoom);
    backup.upKoofFireZoom       = IL2CPP::ReadField<float>(ws, upKoofFireZoom);
    backup.downKoofFirstZoom    = IL2CPP::ReadField<float>(ws, downKoofFirstZoom);
    backup.downKoofZoom         = IL2CPP::ReadField<float>(ws, downKoofZoom);
    backup.moveScatterCoeffZoom = IL2CPP::ReadField<float>(ws, moveScatterCoeffZoom);
    backup.firstShotScatterZoom = IL2CPP::ReadField<bool>(ws, firstShotScatterZoom);
    backup.shootDelay           = IL2CPP::ReadField<float>(ws, shootDelay);
    backup.bulletDelay          = IL2CPP::ReadField<float>(ws, bulletDelay);
    backup.delayTimer           = IL2CPP::ReadField<float>(ws, DelayTimer);
    backup.chargeTime           = IL2CPP::ReadField<float>(ws, chargeTime);
    backup.isSectorsAOE         = IL2CPP::ReadField<bool>(ws, isSectorsAOE);
    backup.sectorsAOEAngleFront = IL2CPP::ReadField<float>(ws, sectorsAOEAngleFront);
    backup.sectorsAOERadius     = IL2CPP::ReadField<float>(ws, sectorsAOERadius);
    backup.hasBackup            = true;
  }

  // Restore original values
  inline void RestoreWeaponSounds(void* ws)
  {
    if (!ws || !backup.hasBackup)
      return;

    using namespace Offsets::WeaponSounds;
    IL2CPP::WriteField(ws, tekKoof, backup.tekKoof);
    IL2CPP::WriteField(ws, upKoofFire, backup.upKoofFire);
    IL2CPP::WriteField(ws, downKoofFirst, backup.downKoofFirst);
    IL2CPP::WriteField(ws, downKoof, backup.downKoof);
    IL2CPP::WriteField(ws, moveScatterCoeff, backup.moveScatterCoeff);
    IL2CPP::WriteField(ws, firstShotScatter, backup.firstShotScatter);
    IL2CPP::WriteField(ws, recoilCoeffZoom, backup.recoilCoeffZoom);
    IL2CPP::WriteField(ws, upKoofFireZoom, backup.upKoofFireZoom);
    IL2CPP::WriteField(ws, downKoofFirstZoom, backup.downKoofFirstZoom);
    IL2CPP::WriteField(ws, downKoofZoom, backup.downKoofZoom);
    IL2CPP::WriteField(ws, moveScatterCoeffZoom, backup.moveScatterCoeffZoom);
    IL2CPP::WriteField(ws, firstShotScatterZoom, backup.firstShotScatterZoom);
    IL2CPP::WriteField(ws, shootDelay, backup.shootDelay);
    IL2CPP::WriteField(ws, bulletDelay, backup.bulletDelay);
    IL2CPP::WriteField(ws, DelayTimer, backup.delayTimer);
    IL2CPP::WriteField(ws, chargeTime, backup.chargeTime);
    IL2CPP::WriteField(ws, isSectorsAOE, backup.isSectorsAOE);
    IL2CPP::WriteField(ws, sectorsAOEAngleFront, backup.sectorsAOEAngleFront);
    IL2CPP::WriteField(ws, sectorsAOERadius, backup.sectorsAOERadius);
  }

  // Apply No Recoil
  inline void ApplyNoRecoil(void* ws)
  {
    if (!ws)
      return;
    using namespace Offsets::WeaponSounds;

    IL2CPP::WriteField(ws, tekKoof, 0.0f);
    IL2CPP::WriteField(ws, upKoofFire, 0.0f);
    IL2CPP::WriteField(ws, downKoofFirst, 0.0f);
    IL2CPP::WriteField(ws, downKoof, 0.0f);
    IL2CPP::WriteField(ws, recoilCoeffZoom, 0.0f);
    IL2CPP::WriteField(ws, upKoofFireZoom, 0.0f);
  }

  // Apply No Spread
  inline void ApplyNoSpread(void* ws)
  {
    if (!ws)
      return;
    using namespace Offsets::WeaponSounds;

    IL2CPP::WriteField(ws, tekKoof, 0.0f);
    IL2CPP::WriteField(ws, upKoofFire, 0.0f);
    IL2CPP::WriteField(ws, downKoofFirst, 0.0f);
    IL2CPP::WriteField(ws, downKoof, 0.0f);
    IL2CPP::WriteField(ws, moveScatterCoeff, 0.0f);
    IL2CPP::WriteField(ws, firstShotScatter, false);
    // Zoom
    IL2CPP::WriteField(ws, upKoofFireZoom, 0.0f);
    IL2CPP::WriteField(ws, downKoofFirstZoom, 0.0f);
    IL2CPP::WriteField(ws, downKoofZoom, 0.0f);
    IL2CPP::WriteField(ws, moveScatterCoeffZoom, 0.0f);
    IL2CPP::WriteField(ws, firstShotScatterZoom, false);
    IL2CPP::WriteField(ws, recoilCoeffZoom, 0.0f);
  }

  // Apply Rapid Fire
  inline void ApplyRapidFire(void* ws)
  {
    if (!ws)
      return;
    using namespace Offsets::WeaponSounds;

    IL2CPP::WriteField(ws, shootDelay, 0.0f);
    IL2CPP::WriteField(ws, bulletDelay, 0.0f);
    IL2CPP::WriteField(ws, DelayTimer, 0.0f);
  }

  // Apply AOE Bullets
  inline void ApplyAOEBullets(void* ws)
  {
    if (!ws)
      return;
    using namespace Offsets::WeaponSounds;

    IL2CPP::WriteField(ws, isSectorsAOE, true);
    IL2CPP::WriteField(ws, sectorsAOEAngleFront, 360.0f);
    IL2CPP::WriteField(ws, sectorsAOEAngleBack, 360.0f);
    IL2CPP::WriteField(ws, sectorsAOERadius, Features::fAOERadius);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultFront, 1.0f);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultSide, 1.0f);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultBack, 1.0f);

    // Also boost explosion radius for explosive weapons
    if (IL2CPP::ReadField<bool>(ws, bazooka)) {
      IL2CPP::WriteField(ws, bazookaExplosionRadius, Features::fAOERadius);
    }
  }

  // Apply Instant Charge
  inline void ApplyInstantCharge(void* ws)
  {
    if (!ws)
      return;
    using namespace Offsets::WeaponSounds;

    if (IL2CPP::ReadField<bool>(ws, isCharging)) {
      IL2CPP::WriteField(ws, chargeTime, 0.001f);
    }
  }

  // Main tick — called every frame
  inline void Tick()
  {
    auto ws = IL2CPP::GetCurrentWeaponSounds();
    if (!ws)
      return;

    // Always backup when weapon changes
    BackupWeaponSounds(ws);

    // Apply combat features
    if (Features::bNoRecoil)
      ApplyNoRecoil(ws);

    if (Features::bNoSpread)
      ApplyNoSpread(ws);

    if (Features::bRapidFire)
      ApplyRapidFire(ws);

    if (Features::bAOEBullets)
      ApplyAOEBullets(ws);

    if (Features::bInstantCharge)
      ApplyInstantCharge(ws);

    // Restore if all combat features disabled and we have backup
    if (
      !Features::bNoRecoil && !Features::bNoSpread && !Features::bRapidFire
      && !Features::bAOEBullets && !Features::bInstantCharge && backup.hasBackup
    ) {
      RestoreWeaponSounds(ws);
      backup.hasBackup = false;
      lastWeaponSounds = nullptr;
    }
  }
}  // namespace Combat
