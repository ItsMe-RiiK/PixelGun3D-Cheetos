#pragma once

namespace WeaponMod {
  namespace Settings {
    // Infinite Ammo
    inline bool bInfiniteAmmo = false;

    // Wallshot
    inline bool bWallshot = false;

    // Crit
    inline bool  bCritChance100  = false;
    inline float fCritMultiplier = 1.0f;

    // Reach
    inline bool  bReach           = false;
    inline float fReachMultiplier = 1.0f;

    // AOE Bullets
    inline bool  bAOEBullets = false;
    inline float fAOERadius  = 50.0f;

    // Auto Headshot
    inline bool bAutoHeadshot = false;
  }  // namespace Settings

  struct WeaponModBackup
  {
    bool  isUnlimitedAmmo;
    int   ammoInClip;
    bool  isWallBreaking;
    float wallBreakingDamageMultiplier;
    bool  bulletBreakout;
    bool  bulletSuperBreakout;
    bool  isPiercingMelee;
    float distancePiercingMelee;
    int   criticalHitChance;
    float criticalHitCoef;
    float radiusRoundMelee;
    float lengthShootFromPrefab;
    float shootDistanceIfZoom;

    // AOE Bullets
    bool  isSectorsAOE;
    float sectorsAOEAngleFront;
    float sectorsAOEAngleBack;
    float sectorsAOERadius;
    float sectorsAOEDmgMultFront;
    float sectorsAOEDmgMultSide;
    float sectorsAOEDmgMultBack;
    float bazookaExplosionRadius;

    bool hasBackup;
  };

  extern WeaponModBackup backup;
  extern void*           lastWeaponSounds;

  void InitMenu();
  void Tick();

  // Logic functions
  void ApplyInfiniteAmmo(void* ws);
  void ApplyWallshot(void* ws);
  void ApplyCritChance(void* ws);
  void ApplyReach(void* ws);
  void ApplyAOEBullets(void* ws);
  void ApplyAutoHeadshot(void* thisPtr, int& damageType);
}  // namespace WeaponMod
