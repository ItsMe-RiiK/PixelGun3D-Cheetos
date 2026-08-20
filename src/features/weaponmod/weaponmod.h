#pragma once

namespace WeaponMod {
  namespace Settings {
    inline bool  bInfiniteAmmo    = false;
    inline bool  bWallshot        = false;
    inline bool  bCritChance100   = false;
    inline float fCritMultiplier  = 5.0f;
    inline bool  bReach           = false;
    inline float fReachMultiplier = 5.0f;
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
    bool  hasBackup;
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
}  // namespace WeaponMod
