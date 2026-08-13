#pragma once
// =====================================================
// Menu — D3D11 Present hook + ImGui primitive overlay
// Keyboard-navigated (Arrow keys)
// =====================================================

#include <d3d11.h>
#include <dxgi.h>

namespace Menu {
  // D3D11 state
  extern ID3D11Device*           pDevice;
  extern ID3D11DeviceContext*    pContext;
  extern ID3D11RenderTargetView* pRenderTarget;
  extern IDXGISwapChain*         pSwapChain;

  // Window
  extern HWND    hGameWindow;
  extern WNDPROC oWndProc;

  // State
  extern bool initialized;

  bool InitD3D11(IDXGISwapChain* sc);
  void Render();
  void Shutdown();
}  // namespace Menu
