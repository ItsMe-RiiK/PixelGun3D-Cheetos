#pragma once
// =====================================================
// ImGui Menu — D3D11 Present hook + ImGui overlay
// =====================================================

#include <d3d11.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "config.h"
#include "features.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// Forward declare the Windows message handler from imgui
extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Menu {
  // D3D11 state
  inline ID3D11Device*           pDevice       = nullptr;
  inline ID3D11DeviceContext*    pContext      = nullptr;
  inline ID3D11RenderTargetView* pRenderTarget = nullptr;
  inline IDXGISwapChain*         pSwapChain    = nullptr;

  // Window
  inline HWND    hGameWindow = nullptr;
  inline WNDPROC oWndProc    = nullptr;

  // State
  inline bool initialized = false;

  // Style setup
  inline void SetupStyle()
  {
    ImGuiStyle& style  = ImGui::GetStyle();
    ImVec4*     colors = style.Colors;

    // Dark theme with purple/cyan accent (Pixel Gun vibe)
    style.WindowRounding    = 8.0f;
    style.FrameRounding     = 4.0f;
    style.GrabRounding      = 4.0f;
    style.TabRounding       = 4.0f;
    style.ScrollbarRounding = 6.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 0.0f;
    style.PopupBorderSize   = 1.0f;
    style.WindowPadding     = ImVec2(12, 12);
    style.FramePadding      = ImVec2(8, 4);
    style.ItemSpacing       = ImVec2(8, 6);
    style.ItemInnerSpacing  = ImVec2(6, 4);

    colors[ImGuiCol_WindowBg]         = ImVec4(0.06f, 0.06f, 0.10f, 0.94f);
    colors[ImGuiCol_ChildBg]          = ImVec4(0.08f, 0.08f, 0.13f, 0.90f);
    colors[ImGuiCol_PopupBg]          = ImVec4(0.08f, 0.08f, 0.13f, 0.94f);
    colors[ImGuiCol_Border]           = ImVec4(0.40f, 0.20f, 0.80f, 0.50f);
    colors[ImGuiCol_FrameBg]          = ImVec4(0.12f, 0.12f, 0.20f, 0.80f);
    colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.20f, 0.15f, 0.35f, 0.80f);
    colors[ImGuiCol_FrameBgActive]    = ImVec4(0.30f, 0.20f, 0.50f, 0.80f);
    colors[ImGuiCol_TitleBg]          = ImVec4(0.08f, 0.06f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive]    = ImVec4(0.15f, 0.10f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.10f, 0.75f);
    colors[ImGuiCol_Tab]              = ImVec4(0.15f, 0.10f, 0.30f, 0.86f);
    colors[ImGuiCol_TabHovered]       = ImVec4(0.35f, 0.20f, 0.70f, 0.80f);
    colors[ImGuiCol_TabActive]        = ImVec4(0.25f, 0.15f, 0.55f, 1.00f);
    colors[ImGuiCol_CheckMark]        = ImVec4(0.50f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]       = ImVec4(0.40f, 0.60f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]           = ImVec4(0.20f, 0.15f, 0.40f, 0.80f);
    colors[ImGuiCol_ButtonHovered]    = ImVec4(0.30f, 0.20f, 0.55f, 0.80f);
    colors[ImGuiCol_ButtonActive]     = ImVec4(0.40f, 0.25f, 0.70f, 0.80f);
    colors[ImGuiCol_Header]           = ImVec4(0.20f, 0.15f, 0.40f, 0.55f);
    colors[ImGuiCol_HeaderHovered]    = ImVec4(0.30f, 0.20f, 0.55f, 0.80f);
    colors[ImGuiCol_HeaderActive]     = ImVec4(0.35f, 0.25f, 0.65f, 1.00f);
    colors[ImGuiCol_Separator]        = ImVec4(0.40f, 0.20f, 0.80f, 0.30f);
    colors[ImGuiCol_Text]             = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled]     = ImVec4(0.45f, 0.45f, 0.55f, 1.00f);
  }

  // Draw the menu
  inline void Render()
  {
    if (!Features::bMenuOpen)
      return;

    ImGui::SetNextWindowSize(ImVec2(520, 540), ImGuiCond_FirstUseEver);
    ImGui::Begin(
      "Pixel Gun 3D Trainer  |  INSERT to toggle", &Features::bMenuOpen, ImGuiWindowFlags_NoCollapse
    );

    if (ImGui::BeginTabBar("MainTabs")) {
      // ==================== COMBAT ====================
      if (ImGui::BeginTabItem("Combat")) {
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Combat Features");
        ImGui::Separator();

        ImGui::Checkbox("Aimbot", &Features::bAimbot);
        if (Features::bAimbot) {
          ImGui::Indent(16.0f);
          ImGui::SliderFloat("FOV##aim", &Features::fAimbotFOV, 1.0f, 90.0f, "%.1f");
          ImGui::Checkbox("Head Only##aim", &Features::bAimbotHeadOnly);
          ImGui::Unindent(16.0f);
        }

        ImGui::Checkbox("No Recoil", &Features::bNoRecoil);
        ImGui::Checkbox("No Spread", &Features::bNoSpread);
        ImGui::Checkbox("Rapid Fire", &Features::bRapidFire);

        ImGui::Checkbox("AOE Bullets", &Features::bAOEBullets);
        if (Features::bAOEBullets) {
          ImGui::Indent(16.0f);
          ImGui::SliderFloat("AOE Radius", &Features::fAOERadius, 5.0f, 200.0f, "%.0f");
          ImGui::Unindent(16.0f);
        }

        ImGui::Checkbox("Instant Charge", &Features::bInstantCharge);

        ImGui::EndTabItem();
      }

      // ==================== VISUAL ====================
      if (ImGui::BeginTabItem("Visual")) {
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Visual Features");
        ImGui::Separator();

        ImGui::Checkbox("Player ESP", &Features::bPlayerESP);
        if (Features::bPlayerESP) {
          ImGui::Indent(16.0f);
          ImGui::Checkbox("Boxes##esp", &Features::bPlayerESPBoxes);
          ImGui::Checkbox("Health Bars##esp", &Features::bPlayerESPHealth);
          ImGui::Checkbox("Names##esp", &Features::bPlayerESPNames);
          ImGui::Unindent(16.0f);
        }

        ImGui::Checkbox("Skeleton ESP", &Features::bSkeletonESP);

        ImGui::EndTabItem();
      }

      // ==================== WEAPON MOD ====================
      if (ImGui::BeginTabItem("Weapon")) {
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Weapon Modifications");
        ImGui::Separator();

        ImGui::Checkbox("Infinite Ammo", &Features::bInfiniteAmmo);
        ImGui::Checkbox("No Reload", &Features::bNoReload);

        ImGui::Checkbox("100%% Crit Chance", &Features::bCritChance100);
        if (Features::bCritChance100) {
          ImGui::Indent(16.0f);
          ImGui::SliderFloat("Crit Multiplier", &Features::fCritMultiplier, 1.0f, 20.0f, "%.1fx");
          ImGui::Unindent(16.0f);
        }

        ImGui::Checkbox("Extended Reach (Melee)", &Features::bReach);
        if (Features::bReach) {
          ImGui::Indent(16.0f);
          ImGui::SliderFloat("Reach Multiplier", &Features::fReachMultiplier, 1.0f, 20.0f, "%.1fx");
          ImGui::Unindent(16.0f);
        }

        ImGui::EndTabItem();
      }

      // ==================== PLAYER MOD ====================
      if (ImGui::BeginTabItem("Player")) {
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Player Modifications");
        ImGui::Separator();

        ImGui::Checkbox("God Mode", &Features::bGodMode);
        ImGui::Checkbox("Auto Heal", &Features::bAutoHeal);

        ImGui::Checkbox("Speed Hack", &Features::bSpeedHack);
        if (Features::bSpeedHack) {
          ImGui::Indent(16.0f);
          ImGui::SliderFloat("Speed Multiplier", &Features::fSpeedMultiplier, 1.0f, 10.0f, "%.1fx");
          ImGui::Unindent(16.0f);
        }

        ImGui::Checkbox("No Fall Damage", &Features::bNoFallDamage);
        ImGui::Checkbox("Invisibility", &Features::bInvisibility);

        ImGui::EndTabItem();
      }

      // ==================== SETTINGS ====================
      if (ImGui::BeginTabItem("Settings")) {
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "System Settings");
        ImGui::Separator();

        ImGui::Checkbox("Anti-Cheat Bypass", &Features::bAntiCheatBypass);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Save Config", ImVec2(150, 30)))
          Config::Save();

        ImGui::SameLine();

        if (ImGui::Button("Load Config", ImVec2(150, 30)))
          Config::Load();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.5f, 1.0f), "Pixel Gun 3D Trainer v1.0");
        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.5f, 1.0f), "Press INSERT to toggle menu");
        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.5f, 1.0f), "Press END to unload");

        ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
    }

    ImGui::End();
  }

  // WndProc hook for input handling
  inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    // Toggle menu with INSERT key
    if (msg == WM_KEYDOWN && wParam == VK_INSERT) {
      Features::bMenuOpen = !Features::bMenuOpen;
      return 0;
    }

    // Forward to ImGui when menu is open
    if (Features::bMenuOpen) {
      if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return 0;

      // Block game input when menu is open
      switch (msg) {
      case WM_LBUTTONDOWN :
      case WM_LBUTTONUP :
      case WM_RBUTTONDOWN :
      case WM_RBUTTONUP :
      case WM_MBUTTONDOWN :
      case WM_MBUTTONUP :
      case WM_MOUSEWHEEL :
      case WM_MOUSEMOVE :
        if (ImGui::GetIO().WantCaptureMouse)
          return 0;
        break;
      case WM_KEYDOWN :
      case WM_KEYUP :
      case WM_CHAR :
        if (ImGui::GetIO().WantCaptureKeyboard)
          return 0;
        break;
      }
    }

    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
  }

  // Initialize D3D11 from the swap chain
  inline bool InitD3D11(IDXGISwapChain* sc)
  {
    pSwapChain = sc;

    if (FAILED(sc->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice))))
      return false;

    pDevice->GetImmediateContext(&pContext);

    // Get back buffer
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(sc->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
      return false;

    pDevice->CreateRenderTargetView(backBuffer, nullptr, &pRenderTarget);
    backBuffer->Release();

    // Get window handle
    DXGI_SWAP_CHAIN_DESC desc;
    sc->GetDesc(&desc);
    hGameWindow = desc.OutputWindow;

    // Hook WndProc
    oWndProc = reinterpret_cast<WNDPROC>(
      SetWindowLongPtr(hGameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc))
    );

    // Init ImGui
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;  // Don't save imgui.ini

    ImGui_ImplWin32_Init(hGameWindow);
    ImGui_ImplDX11_Init(pDevice, pContext);

    SetupStyle();

    initialized = true;
    return true;
  }

  // Cleanup
  inline void Shutdown()
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
