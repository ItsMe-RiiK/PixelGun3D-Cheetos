#pragma once
#include <cstdint>

namespace PlayerMod {
  namespace Settings {
    inline bool  bHighJump       = false;
    inline float fJumpMultiplier = 2.0f;
    inline bool  bFly            = false;
    inline bool  bGodMode        = false;
    inline bool  bAutoHeal       = false;
  }  // namespace Settings

  void InitMenu();
  void Tick();

  // Logic functions
  void ApplyHighJump();
  void ApplyFly();
  void ApplyAutoHeal();

  // Hook overrides
  bool OnApplyDamage(void* thisPtr);
  bool OnEventFired(void* thisPtr, uint8_t eventCode);
}  // namespace PlayerMod
