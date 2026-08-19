#pragma once
// =====================================================
// Hooks — MinHook-based function hooking Header
// =====================================================

#include <windows.h>

namespace Hooks {
  // ---- ApplyDamage Hook (God Mode) ----
  using fn_ApplyDamage = void (*)(
    void*       thisPtr,
    float       damage,
    void*       attacker,
    void*       collider,
    void*       hitPoint,
    int         damageType,
    int         typeDead,
    const char* weaponId,
    int         someInt,
    void*       someEnum,
    void*       methodInfo
  );
  extern fn_ApplyDamage oApplyDamage;

  void hkApplyDamage(
    void*       thisPtr,
    float       damage,
    void*       attacker,
    void*       collider,
    void*       hitPoint,
    int         damageType,
    int         typeDead,
    const char* weaponId,
    int         someInt,
    void*       someEnum,
    void*       methodInfo
  );

  // ---- MinusLive Hook (God Mode alternative) ----
  using fn_MinusLive =
    void (*)(void* thisPtr, float damage, bool isHeadshot, int weaponType, void* methodInfo);
  extern fn_MinusLive oMinusLive;

  void hkMinusLive(void* thisPtr, float damage, bool isHeadshot, int weaponType, void* methodInfo);

  // ---- MinusLiveReal Hook ----
  using fn_MinusLiveReal =
    void (*)(void* thisPtr, float damage, bool isHeadshot, int weaponType, void* methodInfo);
  extern fn_MinusLiveReal oMinusLiveReal;

  void
  hkMinusLiveReal(void* thisPtr, float damage, bool isHeadshot, int weaponType, void* methodInfo);

  // ---- Speed Hack Hook ----
  using fn_MoveSpeedMultiplier = float (*)(void* thisPtr, void* methodInfo);
  extern fn_MoveSpeedMultiplier oMoveSpeedMultiplier;

  float hkMoveSpeedMultiplier(void* thisPtr, void* methodInfo);

  // ---- CheatDetectedBanner Bypass ----
  using fn_CBD_Trigger = void (*)();
  extern fn_CBD_Trigger oCBD_Trigger;
  void                  hkCBD_Trigger();

  using fn_CBD_Show = void (*)();
  extern fn_CBD_Show oCBD_Show;
  void               hkCBD_Show();

  // ---- D3D11 Present Hook (for ImGui rendering) ----
  using fn_Present = HRESULT(WINAPI*)(void* pSwapChain, UINT SyncInterval, UINT Flags);
  extern fn_Present oPresent;

  // Forward declare — implemented in menu.cpp or dllmain.cpp
  HRESULT WINAPI hkPresent(void* pSwapChain, UINT SyncInterval, UINT Flags);

  // ---- Initialize all hooks ----
  bool Init();
  void Shutdown();
}  // namespace Hooks
