#pragma once
// =====================================================
// Feature Toggles & State
// Centralized feature state for the menu and modules
// =====================================================

namespace Features {
  // Combat
  inline bool  bAimbot         = false;
  inline float fAimbotFOV      = 15.0f;
  inline bool  bAimbotHeadOnly = true;
  inline bool  bNoRecoil       = false;
  inline bool  bNoSpread       = false;
  inline bool  bRapidFire      = false;
  inline bool  bAOEBullets     = false;
  inline float fAOERadius      = 50.0f;
  inline bool  bInstantCharge  = false;

  // Visual
  inline bool bPlayerESP       = false;
  inline bool bPlayerESPBoxes  = true;
  inline bool bPlayerESPHealth = true;
  inline bool bPlayerESPNames  = true;
  inline bool bSkeletonESP     = false;

  // Weapon Mod
  inline bool  bInfiniteAmmo    = false;
  inline bool  bNoReload        = false;
  inline bool  bCritChance100   = false;
  inline float fCritMultiplier  = 5.0f;
  inline bool  bReach           = false;
  inline float fReachMultiplier = 5.0f;

  // Player Mod
  inline bool  bSpeedHack       = false;
  inline float fSpeedMultiplier = 2.0f;
  inline bool  bInvisibility    = false;
  inline bool  bNoFallDamage    = false;
  inline bool  bGodMode         = false;
  inline bool  bAutoHeal        = false;

  // System
  inline bool bAntiCheatBypass = true;  // on by default
  inline bool bMenuOpen        = true;
}  // namespace Features
