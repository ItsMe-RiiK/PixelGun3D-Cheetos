#pragma once

namespace Combat {
  namespace Settings {
    inline bool  bAOEBullets = false;
    inline float fAOERadius  = 50.0f;
  }  // namespace Settings

  void InitMenu();
  void Tick();

  // Logic functions
  void ApplyAOEBullets(void* ws);
}  // namespace Combat
