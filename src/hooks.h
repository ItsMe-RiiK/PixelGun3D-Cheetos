#pragma once
// =====================================================
// Hooks — MinHook-based function hooking
// =====================================================

#include "MinHook.h"
#include "features.h"
#include "il2cpp.h"

#include <Windows.h>

namespace Hooks {
  // ---- ApplyDamage Hook (God Mode) ----
  // Original: void PlayerDamageable::ApplyDamage(float damage, IDamageable attacker, Collider col, Vector3 hitPoint, ...)
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
    void*       someEnum
  );
  inline fn_ApplyDamage oApplyDamage = nullptr;

  inline void hkApplyDamage(
    void*       thisPtr,
    float       damage,
    void*       attacker,
    void*       collider,
    void*       hitPoint,
    int         damageType,
    int         typeDead,
    const char* weaponId,
    int         someInt,
    void*       someEnum
  )
  {
    if (Features::bGodMode) {
      // Check if this PlayerDamageable belongs to the local player
      auto localPMC = IL2CPP::GetLocalPlayerMoveC();
      if (localPMC && thisPtr) {
        auto thisPMC = IL2CPP::ReadField<void*>(thisPtr, Offsets::PlayerDamageable::playerMoveC);
        if (thisPMC == localPMC) {
          // Skip damage for local player
          return;
        }
      }
    }

    // Call original
    oApplyDamage(
      thisPtr, damage, attacker, collider, hitPoint, damageType, typeDead, weaponId, someInt,
      someEnum
    );
  }

  // ---- CheatDetectedBanner Bypass ----
  using fn_CBD_Trigger               = void (*)();
  inline fn_CBD_Trigger oCBD_Trigger = nullptr;
  inline void           hkCBD_Trigger() { /* NOP — do nothing */ }

  using fn_CBD_Show            = void (*)();
  inline fn_CBD_Show oCBD_Show = nullptr;
  inline void        hkCBD_Show() { /* NOP — do nothing */ }

  // ---- D3D11 Present Hook (for ImGui rendering) ----
  using fn_Present           = HRESULT(WINAPI*)(void* pSwapChain, UINT SyncInterval, UINT Flags);
  inline fn_Present oPresent = nullptr;

  // Forward declare — implemented in menu.cpp
  HRESULT WINAPI hkPresent(void* pSwapChain, UINT SyncInterval, UINT Flags);

  // ---- Initialize all hooks ----
  inline bool Init()
  {
    if (MH_Initialize() != MH_OK)
      return false;

    auto gaBase = IL2CPP::gameAssemblyBase;

    // Hook PlayerDamageable::ApplyDamage
    auto applyDamageAddr =
      reinterpret_cast<void*>(gaBase + Offsets::PlayerDamageable::ApplyDamage_RVA);
    MH_CreateHook(applyDamageAddr, &hkApplyDamage, reinterpret_cast<void**>(&oApplyDamage));

    // Hook CheatDetectedBanner trigger (anti-cheat bypass)
    auto cbdTriggerAddr = reinterpret_cast<void*>(gaBase + Offsets::AntiCheat::CBD_Trigger_RVA);
    MH_CreateHook(cbdTriggerAddr, &hkCBD_Trigger, reinterpret_cast<void**>(&oCBD_Trigger));

    auto cbdShowAddr = reinterpret_cast<void*>(gaBase + Offsets::AntiCheat::CBD_ShowBanner_RVA);
    MH_CreateHook(cbdShowAddr, &hkCBD_Show, reinterpret_cast<void**>(&oCBD_Show));

    // Enable all hooks
    MH_EnableHook(MH_ALL_HOOKS);

    return true;
  }

  inline void Shutdown()
  {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
  }
}  // namespace Hooks
