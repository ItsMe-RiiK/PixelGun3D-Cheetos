#include "weaponmod.h"
#include "../../utils/il2cpp.h"
#include "../../ui/menu.h"

namespace WeaponMod {

  WeaponModBackup backup           = {};
  void*           lastWeaponSounds = nullptr;

  void BackupWeaponSounds(void* ws)
  {
    if (!ws || ws == lastWeaponSounds)
      return;
    lastWeaponSounds = ws;

    using namespace Offsets::WeaponSounds;

    // Ammo
    static size_t isUnlimitedAmmoOffset = 0;
    if (isUnlimitedAmmoOffset == 0) {
      isUnlimitedAmmoOffset =
        IL2CPP::GetFieldOffset((void*) Offsets::Classes::WeaponSounds, "isUnlimitedAmmo");
      if (isUnlimitedAmmoOffset == 0)
        isUnlimitedAmmoOffset = Offsets::WeaponSounds::isUnlimitedAmmo;
    }
    backup.isUnlimitedAmmo = IL2CPP::ReadField<bool>(ws, isUnlimitedAmmoOffset);
    backup.ammoInClip      = IL2CPP::ReadField<int>(ws, Offsets::WeaponSounds::ammoInClip);

    // Wallshot
    void*         wsClass              = (void*) Offsets::Classes::WeaponSounds;
    static size_t isWallBreakingOffset = IL2CPP::GetFieldOffset(wsClass, "isWallBraking");
    if (!isWallBreakingOffset)
      isWallBreakingOffset = IL2CPP::GetFieldOffset(wsClass, "wallBraking");
    if (isWallBreakingOffset)
      backup.isWallBreaking = IL2CPP::ReadField<bool>(ws, isWallBreakingOffset);

    static size_t wallBreakingDamageMultiplierOffset =
      IL2CPP::GetFieldOffset(wsClass, "wallBrakingDamageMultiplier");
    if (!wallBreakingDamageMultiplierOffset)
      wallBreakingDamageMultiplierOffset =
        IL2CPP::GetFieldOffset(wsClass, "damageWallBrakingMultiplier");
    if (!wallBreakingDamageMultiplierOffset)
      wallBreakingDamageMultiplierOffset =
        IL2CPP::GetFieldOffset(wsClass, "damageMultiplierThroughWall");
    if (!wallBreakingDamageMultiplierOffset)
      wallBreakingDamageMultiplierOffset = IL2CPP::GetFieldOffset(wsClass, "damageWallMultiplier");
    if (wallBreakingDamageMultiplierOffset)
      backup.wallBreakingDamageMultiplier =
        IL2CPP::ReadField<float>(ws, wallBreakingDamageMultiplierOffset);

    static size_t breakoutOffset = IL2CPP::GetFieldOffset(wsClass, "bulletBreakout");
    if (breakoutOffset)
      backup.bulletBreakout = IL2CPP::ReadField<bool>(ws, breakoutOffset);

    static size_t superBreakoutOffset = IL2CPP::GetFieldOffset(wsClass, "bulletSuperBreakout");
    if (superBreakoutOffset)
      backup.bulletSuperBreakout = IL2CPP::ReadField<bool>(ws, superBreakoutOffset);

    backup.isPiercingMelee = IL2CPP::ReadField<bool>(ws, Offsets::WeaponSounds::isPiercingMelee);
    backup.distancePiercingMelee =
      IL2CPP::ReadField<float>(ws, Offsets::WeaponSounds::distancePiercingMelee);

    // Crit
    backup.criticalHitChance = IL2CPP::ReadField<int>(ws, criticalHitChance);
    backup.criticalHitCoef   = IL2CPP::ReadField<float>(ws, criticalHitCoef);

    // Reach
    backup.radiusRoundMelee      = IL2CPP::ReadField<float>(ws, radiusRoundMelee);
    backup.lengthShootFromPrefab = IL2CPP::ReadField<float>(ws, 0x594);
    backup.shootDistanceIfZoom   = IL2CPP::ReadField<float>(ws, 0x4CC);

    backup.hasBackup = true;
  }

  void RestoreWeaponSounds(void* ws)
  {
    if (!ws || !backup.hasBackup)
      return;

    using namespace Offsets::WeaponSounds;

    // Ammo
    static size_t isUnlimitedAmmoOffset = 0;
    if (isUnlimitedAmmoOffset == 0) {
      isUnlimitedAmmoOffset =
        IL2CPP::GetFieldOffset((void*) Offsets::Classes::WeaponSounds, "isUnlimitedAmmo");
      if (isUnlimitedAmmoOffset == 0)
        isUnlimitedAmmoOffset = Offsets::WeaponSounds::isUnlimitedAmmo;
    }
    IL2CPP::WriteField(ws, isUnlimitedAmmoOffset, backup.isUnlimitedAmmo);
    IL2CPP::WriteField(ws, Offsets::WeaponSounds::ammoInClip, backup.ammoInClip);

    // Wallshot
    void*         wsClass              = (void*) Offsets::Classes::WeaponSounds;
    static size_t isWallBreakingOffset = IL2CPP::GetFieldOffset(wsClass, "isWallBraking");
    if (!isWallBreakingOffset)
      isWallBreakingOffset = IL2CPP::GetFieldOffset(wsClass, "wallBraking");
    if (isWallBreakingOffset)
      IL2CPP::WriteField(ws, isWallBreakingOffset, backup.isWallBreaking);

    static size_t wallBreakingDamageMultiplierOffset =
      IL2CPP::GetFieldOffset(wsClass, "wallBrakingDamageMultiplier");
    if (!wallBreakingDamageMultiplierOffset)
      wallBreakingDamageMultiplierOffset =
        IL2CPP::GetFieldOffset(wsClass, "damageWallBrakingMultiplier");
    if (!wallBreakingDamageMultiplierOffset)
      wallBreakingDamageMultiplierOffset =
        IL2CPP::GetFieldOffset(wsClass, "damageMultiplierThroughWall");
    if (!wallBreakingDamageMultiplierOffset)
      wallBreakingDamageMultiplierOffset = IL2CPP::GetFieldOffset(wsClass, "damageWallMultiplier");
    if (wallBreakingDamageMultiplierOffset)
      IL2CPP::WriteField(
        ws, wallBreakingDamageMultiplierOffset, backup.wallBreakingDamageMultiplier
      );

    static size_t breakoutOffset = IL2CPP::GetFieldOffset(wsClass, "bulletBreakout");
    if (breakoutOffset)
      IL2CPP::WriteField(ws, breakoutOffset, backup.bulletBreakout);

    static size_t superBreakoutOffset = IL2CPP::GetFieldOffset(wsClass, "bulletSuperBreakout");
    if (superBreakoutOffset)
      IL2CPP::WriteField(ws, superBreakoutOffset, backup.bulletSuperBreakout);

    IL2CPP::WriteField(ws, Offsets::WeaponSounds::isPiercingMelee, backup.isPiercingMelee);
    IL2CPP::WriteField(
      ws, Offsets::WeaponSounds::distancePiercingMelee, backup.distancePiercingMelee
    );

    // Crit
    IL2CPP::WriteField(ws, criticalHitChance, backup.criticalHitChance);
    IL2CPP::WriteField(ws, criticalHitCoef, backup.criticalHitCoef);

    // Reach
    IL2CPP::WriteField(ws, radiusRoundMelee, backup.radiusRoundMelee);
    IL2CPP::WriteField(ws, 0x594, backup.lengthShootFromPrefab);
    IL2CPP::WriteField(ws, 0x4CC, backup.shootDistanceIfZoom);
  }

  void Tick()
  {
    try {
      auto ws = IL2CPP::GetCurrentWeaponSounds();
      if (!ws)
        return;

      BackupWeaponSounds(ws);

      if (backup.hasBackup) {
        RestoreWeaponSounds(ws);
      }

      bool anyActive = false;

      if (Settings::bInfiniteAmmo) {
        ApplyInfiniteAmmo(ws);
        anyActive = true;
      }

      if (Settings::bWallshot) {
        ApplyWallshot(ws);
        anyActive = true;
      }

      if (Settings::bCritChance100) {
        ApplyCritChance(ws);
        anyActive = true;
      }

      if (Settings::bReach) {
        ApplyReach(ws);
        anyActive = true;
      }

      if (!anyActive) {
        backup.hasBackup = false;
        lastWeaponSounds = nullptr;
      }
    } catch (...) {
      // Prevent crash
    }
  }

  void InitMenu()
  {
    Menu::AddMenuItem({"-- WEAPON --", Menu::ItemType::Header});
    Menu::AddMenuItem({"Infinite Ammo", Menu::ItemType::Bool, &Settings::bInfiniteAmmo});
    Menu::AddMenuItem({"Wallshot", Menu::ItemType::Bool, &Settings::bWallshot});
    Menu::AddMenuItem({"100% Crit Chance", Menu::ItemType::Bool, &Settings::bCritChance100});
    Menu::AddMenuItem(
      {"Crit Multiplier", Menu::ItemType::Float, nullptr, &Settings::fCritMultiplier, 1.0f, 20.0f,
       1.0f}
    );
    Menu::AddMenuItem({"Extended Reach", Menu::ItemType::Bool, &Settings::bReach});
    Menu::AddMenuItem(
      {"Reach Multiplier", Menu::ItemType::Float, nullptr, &Settings::fReachMultiplier, 1.0f, 20.0f,
       1.0f}
    );
  }
}  // namespace WeaponMod
