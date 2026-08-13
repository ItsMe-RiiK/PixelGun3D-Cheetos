#pragma once
// =====================================================
// Visual Features — Player ESP (Boxes, Health, Names)
// Skeleton ESP
// =====================================================

#include <vector>
#include <mutex>

namespace Visual {
  struct Vector3
  {
    float x, y, z;
  };
  struct Vector2
  {
    float x, y;
  };

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

  extern std::vector<PlayerESPData> cachedPlayers;
  extern std::mutex                 espMutex;
  extern float                      g_screenW;
  extern float                      g_screenH;

  bool ResolveUnityMethods();
  void TickMainThread();
  void RenderOverlay(void* pDrawList);
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
  );
}  // namespace Visual
