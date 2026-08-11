#pragma once
// =====================================================
// Visual Features — Player ESP (Boxes, Health, Names)
// Skeleton ESP
// Uses ImGui DrawList for rendering overlays
// =====================================================

#include "../features.h"
#include "../il2cpp.h"

#include <cmath>
#include <vector>

// We use imgui draw list to render — forward declared, linked at compile time
struct ImDrawList;
struct ImVec2;
struct ImColor;

namespace Visual {
  // Minimal Unity vector types for our use
  struct Vector3
  {
    float x, y, z;
  };
  struct Vector2
  {
    float x, y;
  };

  // Player info cache for rendering
  struct PlayerESPData
  {
    Vector2 screenPos;
    Vector2 screenHead;
    float   health;
    float   maxHealth;
    float   distance;
    bool    isEnemy;
    bool    isDead;
    bool    isVisible;
    char    name[64];
  };

  inline std::vector<PlayerESPData> cachedPlayers;

  // ---- Unity interop helpers ----
  // These call Unity engine methods to project world -> screen

  // Typedef for Camera.WorldToScreenPoint
  // Vector3 Camera::WorldToScreenPoint(Vector3 position)
  using fn_WorldToScreenPoint                      = Vector3 (*)(void* camera, Vector3 worldPos);
  inline fn_WorldToScreenPoint pWorldToScreenPoint = nullptr;

  // Typedef for Transform.get_position
  // Vector3 Transform::get_position()
  using fn_GetPosition               = Vector3 (*)(void* transform);
  inline fn_GetPosition pGetPosition = nullptr;

  // Typedef for Camera.get_main
  using fn_GetMainCamera                 = void* (*) ();
  inline fn_GetMainCamera pGetMainCamera = nullptr;

  // Resolve Unity engine methods (called once during init)
  inline bool ResolveUnityMethods()
  {
    auto hUnity = GetModuleHandleA("UnityPlayer.dll");
    if (!hUnity)
      return false;

    // These are resolved via IL2CPP since Camera/Transform are in UnityEngine.CoreModule
    // We'll use il2cpp_class_from_name with the UnityEngine image
    auto gaBase = IL2CPP::gameAssemblyBase;

    // For now, we use a simpler approach: find the methods via class resolution
    // Camera is in UnityEngine namespace
    // We need the UnityEngine.CoreModule image
    auto domain = IL2CPP::domain_get();
    if (!domain)
      return false;

    size_t asmCount   = 0;
    auto   assemblies = IL2CPP::domain_get_assemblies(domain, &asmCount);

    void* coreModuleImage = nullptr;
    for (size_t i = 0; i < asmCount; i++) {
      auto img = IL2CPP::assembly_get_image(assemblies[i]);
      if (!img)
        continue;
      const char* name = IL2CPP::image_get_name(img);
      if (name && strstr(name, "UnityEngine.CoreModule")) {
        coreModuleImage = img;
        break;
      }
    }

    if (!coreModuleImage)
      return false;

    // Resolve Camera class
    auto cameraClass = IL2CPP::class_from_name(coreModuleImage, "UnityEngine", "Camera");
    if (!cameraClass)
      return false;

    // Resolve Camera.get_main method
    auto getMainMethod = IL2CPP::class_get_method_from_name(cameraClass, "get_main", 0);
    if (getMainMethod) {
      // In IL2CPP, MethodInfo->methodPointer is at offset 0x0
      pGetMainCamera = *reinterpret_cast<fn_GetMainCamera*>(getMainMethod);
    }

    // Resolve Camera.WorldToScreenPoint
    auto w2sMethod = IL2CPP::class_get_method_from_name(cameraClass, "WorldToScreenPoint", 1);
    if (w2sMethod) {
      pWorldToScreenPoint = *reinterpret_cast<fn_WorldToScreenPoint*>(w2sMethod);
    }

    // Resolve Transform class
    auto transformClass = IL2CPP::class_from_name(coreModuleImage, "UnityEngine", "Transform");
    if (transformClass) {
      auto getPosMethod = IL2CPP::class_get_method_from_name(transformClass, "get_position", 0);
      if (getPosMethod) {
        pGetPosition = *reinterpret_cast<fn_GetPosition*>(getPosMethod);
      }
    }

    return pGetMainCamera != nullptr;
  }

  // Project world position to screen coordinates
  inline bool
  WorldToScreen(Vector3 worldPos, Vector2& screenOut, void* camera, float screenW, float screenH)
  {
    if (!pWorldToScreenPoint || !camera)
      return false;

    Vector3 screenPos = pWorldToScreenPoint(camera, worldPos);

    // Unity screen coords: (0,0) bottom-left, z = depth
    if (screenPos.z < 0.1f)
      return false;

    // Flip Y for ImGui (top-left origin)
    screenOut.x = screenPos.x;
    screenOut.y = screenH - screenPos.y;

    return (
      screenOut.x >= 0 && screenOut.x <= screenW && screenOut.y >= 0 && screenOut.y <= screenH
    );
  }

  // ---- ImGui-based ESP rendering ----
  // Called from the ImGui render loop with the background draw list

  inline void DrawESP(void* pDrawList, float screenW, float screenH)
  {
    if (!Features::bPlayerESP && !Features::bSkeletonESP)
      return;
    if (!pGetMainCamera)
      return;

    // Get main camera
    void* camera = pGetMainCamera();
    if (!camera)
      return;

    // Get local player for reference
    auto localPMC = IL2CPP::GetLocalPlayerMoveC();
    if (!localPMC)
      return;

    auto localTransform =
      IL2CPP::ReadField<void*>(localPMC, Offsets::PlayerMoveC::myPlayerTransform);
    if (!localTransform || !pGetPosition)
      return;

    Vector3 localPos = pGetPosition(localTransform);

    // We cast to ImDrawList* for drawing
    // The actual ImGui calls happen in menu.cpp which includes imgui.h
    // Here we just collect the data; rendering happens via callback

    // Note: Full player iteration requires finding all Player_move_c instances
    // This typically uses Object.FindObjectsOfType or the Photon player list
    // For now, we provide the framework; iteration will be connected during testing
  }

  // Draw a 2D bounding box around a player
  inline void DrawPlayerBox(
    void*       drawList,
    Vector2     footScreen,
    Vector2     headScreen,
    float       health,
    float       maxHealth,
    bool        isEnemy,
    const char* name,
    float       screenW,
    float       screenH
  )
  {
    // Calculate box dimensions from head/foot screen positions
    float height = footScreen.y - headScreen.y;
    if (height < 5.0f)
      return;

    float width   = height * 0.4f;
    float centerX = (footScreen.x + headScreen.x) * 0.5f;

    float left   = centerX - width * 0.5f;
    float right  = centerX + width * 0.5f;
    float top    = headScreen.y - height * 0.1f;
    float bottom = footScreen.y;

    // Colors
    unsigned int boxColor = isEnemy ? 0xFF0000FF : 0xFF00FF00;  // Red for enemy, Green for ally
    unsigned int healthColor;

    float healthPercent = (maxHealth > 0) ? (health / maxHealth) : 0.0f;
    if (healthPercent > 0.5f)
      healthColor = 0xFF00FF00;  // Green
    else if (healthPercent > 0.25f)
      healthColor = 0xFF00FFFF;  // Yellow
    else
      healthColor = 0xFF0000FF;  // Red

    // Note: Actual ImGui draw calls (AddRect, AddRectFilled, AddText)
    // are done in menu.cpp which has access to ImDrawList*
    // This function provides the layout logic

    // The rendering integration is completed in the menu render loop
  }
}  // namespace Visual
