#include "menu.h"
#include "../utils/settings.h"
#include "../utils/config.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <vector>
#include <string>
#include <algorithm>

namespace Menu {
  ID3D11Device*           pDevice       = nullptr;
  ID3D11DeviceContext*    pContext      = nullptr;
  ID3D11RenderTargetView* pRenderTarget = nullptr;
  IDXGISwapChain*         pSwapChain    = nullptr;

  HWND    hGameWindow = nullptr;
  WNDPROC oWndProc    = nullptr;

  bool initialized = false;

  enum class ItemType {
    Header,
    Bool,
    Float,
    Action
  };

  struct MenuItem
  {
    std::string name;
    ItemType    type;
    bool*       bValue = nullptr;
    float*      fValue = nullptr;
    float       fMin   = 0.0f;
    float       fMax   = 0.0f;
    float       fStep  = 0.0f;
    void (*action)()   = nullptr;
    bool isExpanded    = true;
  };

  std::vector<MenuItem> items;
  int                   selectedIndex = 0;
  int                   scrollOffset  = 0;

  void SetupItems()
  {
    items.clear();
    // COMBAT
    items.push_back({"-- COMBAT --", ItemType::Header});
    items.push_back({"Aimbot", ItemType::Bool, &Features::bAimbot});
    items.push_back(
      {"Aimbot FOV", ItemType::Float, nullptr, &Features::fAimbotFOV, 1.0f, 90.0f, 1.0f}
    );
    items.push_back({"Aimbot Head Only", ItemType::Bool, &Features::bAimbotHeadOnly});
    items.push_back({"No Recoil", ItemType::Bool, &Features::bNoRecoil});
    items.push_back({"No Spread", ItemType::Bool, &Features::bNoSpread});
    items.push_back({"Rapid Fire", ItemType::Bool, &Features::bRapidFire});
    items.push_back({"AOE Bullets", ItemType::Bool, &Features::bAOEBullets});
    items.push_back(
      {"AOE Radius", ItemType::Float, nullptr, &Features::fAOERadius, 5.0f, 200.0f, 5.0f}
    );
    items.push_back({"Instant Charge", ItemType::Bool, &Features::bInstantCharge});

    // VISUAL
    items.push_back({"-- VISUAL --", ItemType::Header});
    items.push_back({"Player ESP", ItemType::Bool, &Features::bPlayerESP});
    items.push_back({"ESP Boxes", ItemType::Bool, &Features::bPlayerESPBoxes});
    items.push_back({"ESP Health", ItemType::Bool, &Features::bPlayerESPHealth});
    items.push_back({"ESP Names", ItemType::Bool, &Features::bPlayerESPNames});
    items.push_back({"Skeleton ESP", ItemType::Bool, &Features::bSkeletonESP});

    // WEAPON
    items.push_back({"-- WEAPON --", ItemType::Header});
    items.push_back({"Infinite Ammo", ItemType::Bool, &Features::bInfiniteAmmo});
    items.push_back({"No Reload", ItemType::Bool, &Features::bNoReload});
    items.push_back({"100% Crit Chance", ItemType::Bool, &Features::bCritChance100});
    items.push_back(
      {"Crit Multiplier", ItemType::Float, nullptr, &Features::fCritMultiplier, 1.0f, 20.0f, 1.0f}
    );
    items.push_back({"Extended Reach", ItemType::Bool, &Features::bReach});
    items.push_back(
      {"Reach Multiplier", ItemType::Float, nullptr, &Features::fReachMultiplier, 1.0f, 20.0f, 1.0f}
    );

    // PLAYER
    items.push_back({"-- PLAYER --", ItemType::Header});
    items.push_back({"God Mode", ItemType::Bool, &Features::bGodMode});
    items.push_back({"Auto Heal", ItemType::Bool, &Features::bAutoHeal});
    items.push_back({"Speed Hack", ItemType::Bool, &Features::bSpeedHack});
    items.push_back(
      {"Speed Multiplier", ItemType::Float, nullptr, &Features::fSpeedMultiplier, 1.0f, 10.0f, 0.5f}
    );
    items.push_back({"No Fall Damage", ItemType::Bool, &Features::bNoFallDamage});
    items.push_back({"Invisibility", ItemType::Bool, &Features::bInvisibility});

    // SETTINGS
    items.push_back({"-- SETTINGS --", ItemType::Header});
    items.push_back({"Anti-Cheat Bypass", ItemType::Bool, &Features::bAntiCheatBypass});
    items.push_back({"Save Config", ItemType::Action, nullptr, nullptr, 0, 0, 0, []() {
                       Config::Save();
                     }});
    items.push_back({"Load Config", ItemType::Action, nullptr, nullptr, 0, 0, 0, []() {
                       Config::Load();
                     }});
  }

  std::vector<int> GetVisibleIndices()
  {
    std::vector<int> visible;
    bool             currentHeaderExpanded = true;
    for (int i = 0; i < items.size(); ++i) {
      if (items[i].type == ItemType::Header) {
        currentHeaderExpanded = items[i].isExpanded;
        visible.push_back(i);
      }
      else if (currentHeaderExpanded) {
        visible.push_back(i);
      }
    }
    return visible;
  }

  void MoveSelection(int dir)
  {
    if (items.empty())
      return;

    std::vector<int> visible = GetVisibleIndices();
    if (visible.empty())
      return;

    // Find current index in visible list
    auto it           = std::find(visible.begin(), visible.end(), selectedIndex);
    int  visibleIndex = 0;
    if (it != visible.end()) {
      visibleIndex = std::distance(visible.begin(), it);
    }

    visibleIndex += dir;
    if (visibleIndex < 0)
      visibleIndex = visible.size() - 1;
    if (visibleIndex >= visible.size())
      visibleIndex = 0;

    selectedIndex = visible[visibleIndex];
  }

  void Interact(int dir)
  {
    if (items.empty() || selectedIndex < 0 || selectedIndex >= items.size())
      return;
    auto& item = items[selectedIndex];

    if (item.type == ItemType::Header) {
      if (dir != 0)
        item.isExpanded = !item.isExpanded;
    }
    else if (item.type == ItemType::Bool && item.bValue) {
      if (dir != 0)
        *item.bValue = !(*item.bValue);
    }
    else if (item.type == ItemType::Float && item.fValue) {
      if (dir > 0) {
        *item.fValue += item.fStep;
        if (*item.fValue > item.fMax)
          *item.fValue = item.fMax;
      }
      else if (dir < 0) {
        *item.fValue -= item.fStep;
        if (*item.fValue < item.fMin)
          *item.fValue = item.fMin;
      }
    }
    else if (item.type == ItemType::Action && item.action) {
      if (dir != 0)
        item.action();
    }
  }

  LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    if (msg == WM_KEYDOWN) {
      if (wParam == VK_INSERT) {
        Features::bMenuOpen = !Features::bMenuOpen;
        return 0;
      }

      if (Features::bMenuOpen) {
        if (wParam == VK_UP) {
          MoveSelection(-1);
          return 0;
        }
        if (wParam == VK_DOWN) {
          MoveSelection(1);
          return 0;
        }
        if (wParam == VK_LEFT) {
          Interact(-1);
          return 0;
        }
        if (wParam == VK_RIGHT) {
          Interact(1);
          return 0;
        }
        if (wParam == VK_RETURN || wParam == VK_SPACE) {
          Interact(1);
          return 0;
        }
      }
    }

    if (Features::bMenuOpen) {
      // Block game input when menu is open so arrows don't move camera
      switch (msg) {
      case WM_LBUTTONDOWN :
      case WM_LBUTTONUP :
      case WM_RBUTTONDOWN :
      case WM_RBUTTONUP :
      case WM_MBUTTONDOWN :
      case WM_MBUTTONUP :
      case WM_MOUSEWHEEL :
      case WM_MOUSEMOVE :
      case WM_KEYDOWN :
      case WM_KEYUP :
      case WM_CHAR :
        return 0;
      }
    }

    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
  }

  bool InitD3D11(IDXGISwapChain* sc)
  {
    pSwapChain = sc;

    if (FAILED(sc->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice))))
      return false;

    pDevice->GetImmediateContext(&pContext);

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(sc->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
      return false;

    pDevice->CreateRenderTargetView(backBuffer, nullptr, &pRenderTarget);
    backBuffer->Release();

    DXGI_SWAP_CHAIN_DESC desc;
    sc->GetDesc(&desc);
    hGameWindow = desc.OutputWindow;

    oWndProc = reinterpret_cast<WNDPROC>(
      SetWindowLongPtr(hGameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc))
    );

    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;

    ImGui_ImplWin32_Init(hGameWindow);
    ImGui_ImplDX11_Init(pDevice, pContext);

    ImGuiStyle& style               = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);

    SetupItems();
    initialized = true;
    return true;
  }

  void Render()
  {
    if (!Features::bMenuOpen)
      return;

    auto drawList = ImGui::GetBackgroundDrawList();

    float screenH = ImGui::GetIO().DisplaySize.y;

    float startX    = 50.0f;
    float startY    = 50.0f;
    float width     = 350.0f;
    float rowHeight = 20.0f;
    float padding   = 10.0f;

    std::vector<int> visibleIndices = GetVisibleIndices();
    if (visibleIndices.empty())
      return;

    // Calculate how many items can fit on screen safely
    float availableHeight =
      screenH - startY - (padding * 2) - 30.0f - 20.0f;  // 20.0f bottom margin
    int maxVisibleItems = static_cast<int>(availableHeight / rowHeight);
    if (maxVisibleItems < 5)
      maxVisibleItems = 5;

    // Find current selected index in visible list
    auto it = std::find(visibleIndices.begin(), visibleIndices.end(), selectedIndex);
    int  selectedVisibleIndex = 0;
    if (it != visibleIndices.end()) {
      selectedVisibleIndex = std::distance(visibleIndices.begin(), it);
    }
    else {
      selectedIndex        = visibleIndices[0];
      selectedVisibleIndex = 0;
    }

    // Scroll logic
    if (selectedVisibleIndex < scrollOffset) {
      scrollOffset = selectedVisibleIndex;
    }
    else if (selectedVisibleIndex >= scrollOffset + maxVisibleItems) {
      scrollOffset = selectedVisibleIndex - maxVisibleItems + 1;
    }

    // Safety clamp
    if (scrollOffset > visibleIndices.size() - maxVisibleItems) {
      scrollOffset = std::max(0, (int) visibleIndices.size() - maxVisibleItems);
    }

    int   itemsToDraw = std::min(maxVisibleItems, (int) visibleIndices.size() - scrollOffset);
    float totalHeight = itemsToDraw * rowHeight + padding * 2 + 30.0f;

    // Draw background
    drawList->AddRectFilled(
      ImVec2(startX, startY), ImVec2(startX + width, startY + totalHeight),
      IM_COL32(20, 20, 25, 230)
    );
    drawList->AddRect(
      ImVec2(startX, startY), ImVec2(startX + width, startY + totalHeight),
      IM_COL32(100, 50, 200, 255), 0, 0, 2.0f
    );

    // Title
    drawList->AddText(
      ImVec2(startX + padding, startY + padding), IM_COL32(100, 200, 255, 255),
      "Pixel Gun 3D Trainer [Arrow Keys to Navigate]"
    );

    float currentY = startY + padding + 30.0f;

    for (int i = 0; i < itemsToDraw; ++i) {
      int   actualIndex = visibleIndices[scrollOffset + i];
      auto& item        = items[actualIndex];
      ImU32 textColor   = IM_COL32(200, 200, 200, 255);

      if (actualIndex == selectedIndex) {
        // Draw selection highlight
        drawList->AddRectFilled(
          ImVec2(startX, currentY), ImVec2(startX + width, currentY + rowHeight),
          IM_COL32(80, 40, 160, 180)
        );
        textColor = IM_COL32(255, 255, 255, 255);
      }

      if (item.type == ItemType::Header) {
        textColor              = IM_COL32(150, 150, 150, 255);
        std::string headerName = item.name + (item.isExpanded ? " [-]" : " [+]");
        drawList->AddText(
          ImVec2(startX + width / 2 - 40, currentY + 2), textColor, headerName.c_str()
        );
      }
      else {
        drawList->AddText(ImVec2(startX + padding, currentY + 2), textColor, item.name.c_str());

        if (item.type == ItemType::Bool && item.bValue) {
          const char* valStr = *item.bValue ? "[ ON ]" : "[ OFF ]";
          ImU32 valColor = *item.bValue ? IM_COL32(50, 255, 50, 255) : IM_COL32(255, 50, 50, 255);
          drawList->AddText(ImVec2(startX + width - 60, currentY + 2), valColor, valStr);
        }
        else if (item.type == ItemType::Float && item.fValue) {
          char buf[32];
          snprintf(buf, sizeof(buf), "< %.2f >", *item.fValue);
          drawList->AddText(
            ImVec2(startX + width - 80, currentY + 2), IM_COL32(255, 255, 100, 255), buf
          );
        }
        else if (item.type == ItemType::Action) {
          drawList->AddText(
            ImVec2(startX + width - 80, currentY + 2), IM_COL32(200, 200, 200, 255), "[ ENTER ]"
          );
        }
      }

      currentY += rowHeight;
    }
  }

  void Shutdown()
  {
    if (!initialized)
      return;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (oWndProc && hGameWindow)
      SetWindowLongPtr(hGameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));

    if (pRenderTarget)
      pRenderTarget->Release();
    if (pContext)
      pContext->Release();
    if (pDevice)
      pDevice->Release();

    initialized = false;
  }
}  // namespace Menu
