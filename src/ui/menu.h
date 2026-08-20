#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include <string>

namespace Menu {
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

  void AddMenuItem(const MenuItem& item);
  bool InitD3D11(IDXGISwapChain* sc);
  void Render();
  void Shutdown();
}  // namespace Menu
