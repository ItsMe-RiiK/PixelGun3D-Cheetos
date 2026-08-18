#include "visual.h"
#include "../utils/settings.h"
#include "../utils/il2cpp.h"
#include <windows.h>
#include <imgui.h>

namespace Visual {
  std::vector<PlayerESPData> cachedPlayers;
  std::mutex                 espMutex;
  float                      g_screenW = 0.0f;
  float                      g_screenH = 0.0f;

  using fn_WorldToScreenPoint               = Vector3 (*)(void* camera, Vector3 worldPos);
  fn_WorldToScreenPoint pWorldToScreenPoint = nullptr;

  using fn_GetPixelWidth          = int (*)(void* camera);
  fn_GetPixelWidth pGetPixelWidth = nullptr;

  using fn_GetPixelHeight           = int (*)(void* camera);
  fn_GetPixelHeight pGetPixelHeight = nullptr;

  using fn_GetPosition        = Vector3 (*)(void* transform);
  fn_GetPosition pGetPosition = nullptr;

  using fn_GetMainCamera          = void* (*) ();
  fn_GetMainCamera pGetMainCamera = nullptr;

  // TextMesh text reading for ESP names
  using fn_GetText    = void* (*) (void* textMesh);
  fn_GetText pGetText = nullptr;

  // Helper: extract C-string from IL2CPP string object
  // IL2CPP string layout: [object header 0x10] [int32 length @ 0x10] [char16_t chars[] @ 0x14]
  static bool ReadIL2CPPString(void* il2cppStr, char* outBuf, size_t outBufSize)
  {
    if (!il2cppStr || outBufSize == 0)
      return false;

    int strLen = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(il2cppStr) + 0x10);
    if (strLen <= 0 || strLen > 256) {
      outBuf[0] = '\0';
      return false;
    }

    auto   chars   = reinterpret_cast<char16_t*>(reinterpret_cast<uintptr_t>(il2cppStr) + 0x14);
    size_t copyLen = (strLen < (outBufSize - 1)) ? strLen : (outBufSize - 1);
    for (size_t i = 0; i < copyLen; ++i) {
      outBuf[i] = (chars[i] < 128) ? static_cast<char>(chars[i]) : '?';
    }
    outBuf[copyLen] = '\0';
    return true;
  }

  bool ResolveUnityMethods()
  {
    auto hUnity = GetModuleHandleA("UnityPlayer.dll");
    if (!hUnity)
      return false;

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

    auto cameraClass = IL2CPP::class_from_name(coreModuleImage, "UnityEngine", "Camera");
    if (!cameraClass)
      return false;

    auto getMainMethod = IL2CPP::class_get_method_from_name(cameraClass, "get_main", 0);
    if (getMainMethod) {
      pGetMainCamera = *reinterpret_cast<fn_GetMainCamera*>(getMainMethod);
    }

    auto w2sMethod = IL2CPP::class_get_method_from_name(cameraClass, "WorldToScreenPoint", 1);
    if (w2sMethod) {
      pWorldToScreenPoint = *reinterpret_cast<fn_WorldToScreenPoint*>(w2sMethod);
    }

    auto getWidthMethod = IL2CPP::class_get_method_from_name(cameraClass, "get_pixelWidth", 0);
    if (getWidthMethod) {
      pGetPixelWidth = *reinterpret_cast<fn_GetPixelWidth*>(getWidthMethod);
    }

    auto getHeightMethod = IL2CPP::class_get_method_from_name(cameraClass, "get_pixelHeight", 0);
    if (getHeightMethod) {
      pGetPixelHeight = *reinterpret_cast<fn_GetPixelHeight*>(getHeightMethod);
    }

    auto transformClass = IL2CPP::class_from_name(coreModuleImage, "UnityEngine", "Transform");
    if (transformClass) {
      auto getPosMethod = IL2CPP::class_get_method_from_name(transformClass, "get_position", 0);
      if (getPosMethod) {
        pGetPosition = *reinterpret_cast<fn_GetPosition*>(getPosMethod);
      }
    }

    // Resolve TextMesh::get_text from TextRenderingModule
    void* textRenderingImage = nullptr;
    for (size_t i = 0; i < asmCount; i++) {
      auto img = IL2CPP::assembly_get_image(assemblies[i]);
      if (!img)
        continue;
      const char* name = IL2CPP::image_get_name(img);
      if (name && strstr(name, "UnityEngine.TextRenderingModule")) {
        textRenderingImage = img;
        break;
      }
    }

    if (textRenderingImage) {
      auto textMeshClass = IL2CPP::class_from_name(textRenderingImage, "UnityEngine", "TextMesh");
      if (textMeshClass) {
        auto getTextMethod = IL2CPP::class_get_method_from_name(textMeshClass, "get_text", 0);
        if (getTextMethod) {
          pGetText = *reinterpret_cast<fn_GetText*>(getTextMethod);
        }
      }
    }

    return pGetMainCamera != nullptr;
  }

  bool
  WorldToScreen(Vector3 worldPos, Vector2& screenOut, void* camera, float screenW, float screenH)
  {
    if (!pWorldToScreenPoint || !camera)
      return false;

    Vector3 screenPos = pWorldToScreenPoint(camera, worldPos);

    if (screenPos.z < 0.1f)
      return false;

    float unityW = screenW;
    float unityH = screenH;

    if (pGetPixelWidth && pGetPixelHeight) {
      int camW = pGetPixelWidth(camera);
      int camH = pGetPixelHeight(camera);
      if (camW > 0 && camH > 0) {
        unityW = static_cast<float>(camW);
        unityH = static_cast<float>(camH);
      }
    }

    screenOut.x = screenPos.x * (screenW / unityW);
    screenOut.y = (unityH - screenPos.y) * (screenH / unityH);

    return (
      screenOut.x >= 0 && screenOut.x <= screenW && screenOut.y >= 0 && screenOut.y <= screenH
    );
  }

  void TickMainThread()
  {
    try {
      if (!Features::bPlayerESP && !Features::bSkeletonESP) {
        std::lock_guard<std::mutex> lock(espMutex);
        cachedPlayers.clear();
        return;
      }

      if (!pGetMainCamera || !pWorldToScreenPoint || !pGetPosition) {
        std::lock_guard<std::mutex> lock(espMutex);
        cachedPlayers.clear();
        return;
      }

      void* camera = pGetMainCamera();
      if (!camera) {
        std::lock_guard<std::mutex> lock(espMutex);
        cachedPlayers.clear();
        return;
      }

      auto localPMC = IL2CPP::GetLocalPlayerMoveC();
      if (!localPMC) {
        std::lock_guard<std::mutex> lock(espMutex);
        cachedPlayers.clear();
        return;
      }

      auto localTransform =
        IL2CPP::ReadField<void*>(localPMC, Offsets::PlayerMoveC::myPlayerTransform);
      if (!localTransform)
        return;

      Vector3 localPos = pGetPosition(localTransform);

      std::vector<PlayerESPData> newCache;
      auto                       players = IL2CPP::GetPlayers();
      for (auto pmc : players) {
        if (!pmc || pmc == localPMC)
          continue;

        auto playerTransform =
          IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::myPlayerTransform);
        if (!playerTransform)
          continue;

        // Skip dead players
        auto damageable = IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::playerDamageable);
        if (damageable) {
          using fn_IsDead = bool (*)(void*);
          auto isDead     = reinterpret_cast<fn_IsDead>(
            IL2CPP::GetMethodAddress(Offsets::PlayerDamageable::IsDead_RVA)
          );
          if (isDead && isDead(damageable))
            continue;
        }

        Vector3 targetPos = pGetPosition(playerTransform);
        auto    headTransform =
          IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::PlayerHeadTransform);
        Vector3 headPos = targetPos;
        if (headTransform) {
          headPos = pGetPosition(headTransform);
          headPos.y += 0.3f;  // Offset to top of head
        }
        else {
          headPos.y += 2.0f;  // Guess height
        }

        Vector2 footScreen, headScreen;
        if (
          !WorldToScreen(targetPos, footScreen, camera, g_screenW, g_screenH)
          || !WorldToScreen(headPos, headScreen, camera, g_screenW, g_screenH)
        ) {
          continue;
        }

        // Is enemy? IsEnemyTo takes Player_move_c* as parameter (not PlayerDamageable*)
        bool isEnemy = true;
        if (damageable) {
          using fn_IsEnemyTo = bool (*)(void*, void*);
          auto isEnemyTo     = reinterpret_cast<fn_IsEnemyTo>(
            IL2CPP::GetMethodAddress(Offsets::PlayerDamageable::IsEnemyTo_RVA)
          );
          if (isEnemyTo) {
            isEnemy = isEnemyTo(damageable, localPMC);
          }
        }

        if (!isEnemy) {
          continue;  // Skip teammates — ESP team feature is deleted
        }

        float health    = 100.0f;
        float maxHealth = 100.0f;

        // Read player name from nickLabel TextMesh
        char playerName[64] = {0};
        if (Features::bPlayerESPNames && pGetText) {
          auto nickLabel = IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::nickLabel);
          if (nickLabel) {
            void* il2cppStr = pGetText(nickLabel);
            ReadIL2CPPString(il2cppStr, playerName, sizeof(playerName));
          }
        }

        PlayerESPData data;
        data.screenPos  = footScreen;
        data.screenHead = headScreen;
        data.health     = health;
        data.maxHealth  = maxHealth;
        data.isEnemy    = isEnemy;
        memcpy(data.name, playerName, sizeof(data.name));
        newCache.push_back(data);
      }

      std::lock_guard<std::mutex> lock(espMutex);
      cachedPlayers = newCache;
    } catch (...) {
      // Catch exceptions (like read access violations) to prevent crashes
    }
  }

  void RenderOverlay(void* pDrawList)
  {
    if (!Features::bPlayerESP && !Features::bSkeletonESP)
      return;

    std::lock_guard<std::mutex> lock(espMutex);
    for (const auto& player : cachedPlayers) {
      if (Features::bPlayerESPBoxes) {
        DrawPlayerBox(
          pDrawList, player.screenPos, player.screenHead, player.health, player.maxHealth,
          player.isEnemy, player.name[0] ? player.name : nullptr, g_screenW, g_screenH
        );
      }
    }
  }

  void DrawPlayerBox(
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
    float height = footScreen.y - headScreen.y;
    if (height < 5.0f)
      return;

    float width   = height * 0.5f;
    float centerX = (footScreen.x + headScreen.x) * 0.5f;

    float left   = centerX - width * 0.5f;
    float right  = centerX + width * 0.5f;
    float top    = headScreen.y;
    float bottom = footScreen.y;

    unsigned int boxColor = isEnemy ? 0xFF0000FF : 0xFF00FF00;

    // Convert ImDrawList pointer and draw
    ImDrawList* dl = reinterpret_cast<ImDrawList*>(drawList);

    dl->AddRect(ImVec2(left, top), ImVec2(right, bottom), boxColor, 0.0f, 0, 1.5f);

    if (Features::bPlayerESPHealth) {
      unsigned int healthColor;
      float        healthPercent = (maxHealth > 0) ? (health / maxHealth) : 0.0f;
      if (healthPercent > 0.5f)
        healthColor = 0xFF00FF00;
      else if (healthPercent > 0.25f)
        healthColor = 0xFF00FFFF;
      else
        healthColor = 0xFF0000FF;

      float hpBarHeight = height * healthPercent;
      dl->AddRectFilled(
        ImVec2(left - 6.0f, bottom - hpBarHeight), ImVec2(left - 2.0f, bottom), healthColor
      );
      dl->AddRect(ImVec2(left - 6.0f, top), ImVec2(left - 2.0f, bottom), 0xFF000000);
    }

    // Draw name above box
    if (Features::bPlayerESPNames && name && name[0]) {
      dl->AddText(ImVec2(centerX - 20.0f, top - 14.0f), 0xFFFFFFFF, name);
    }
  }
}  // namespace Visual
