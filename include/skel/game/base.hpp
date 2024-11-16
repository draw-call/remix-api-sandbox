#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <functional>
#include <mutex>
#include <utility>
#include <iostream>

#include <Windows.h>
#include <d3dx9.h>

#include "kanan.h"
#include "detours.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "dxerror.hpp"
#include "detoursafe.hpp"

#include "skel/game/constants.hpp"
#include "skel/engine/cdc_internal.hpp"

namespace skel::cdc
{

// Callback Types
typedef BOOL (WINAPI *SetCursorPosOrigT)(int, int);
typedef std::function<bool(int, int)>  SetCurrPosCallbackT;
typedef std::function<void(HWND, UINT, WPARAM, LPARAM)> WndProcCallbackT;

// WndProc callback wrapper
static WNDPROC          s_WndProcOrig;
static WndProcCallbackT s_WndProcCallback;
static LRESULT s_ThreadWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  // Call per-game registered WndProc callback
  s_WndProcCallback(hWnd, msg, wParam, lParam);
  return CallWindowProc(s_WndProcOrig, hWnd, msg, wParam, lParam);
}

// SetCursorPos callback wrapper
static SetCursorPosOrigT   s_SetCursorPosOrig;
static SetCurrPosCallbackT s_SetCursorCallback;
static BOOL WINAPI s_LocalSetCursorPos(int X, int Y) {
  return s_SetCursorCallback(X, Y);
}

// GameState base for things common to CDC engine

class GameStateBase {
public:
  GameStateBase() :
    // Public
    world(),
    view(),
    projection(),
    ident(),
    sceneSetPosN(ATOMIC_VAR_INIT(0)),
    inScene(ATOMIC_VAR_INIT(false)),
    renderReady(ATOMIC_VAR_INIT(false)),
    frameNumber(ATOMIC_VAR_INIT(0)),
    frameStartNumber(ATOMIC_VAR_INIT(DefaultFrameStart)),
    isDiagWindowFocused(ATOMIC_VAR_INIT(false)),
    lastConstData(1000), // reserve 1000 elements
    // Protected
    m_GD(nullptr),
    m_Camera(nullptr),
    m_DeviceManager(nullptr),
    m_StateManager(nullptr),
    m_RenderDevice(nullptr),
    m_currentCameraFOV(ATOMIC_VAR_INIT(DefaultFovDist)),
    m_PCDeviceManagerSig(PCDeviceManagerSig),
    m_PCDeviceManagerECXPtrOff(PCDeviceManagerECXPtrOff),
    m_PCDeviceManagerStateManagerOff(PCDeviceManagerStateManagerOff),
    m_CameraAddr(CameraAddr),
    m_GlobalDataAddr(GlobalDataAddr),
    m_PCRenderDeviceAddr(PCRenderDeviceAddr),
    m_InputDisableAddr(InputDisableAddr),
    // Private
    m_curHwnd(NULL),
    m_ImGui_once()
  {
    D3DXMatrixIdentity(&ident);
  };

  ~GameStateBase() {
    // no-op if never setup
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    // tear down registered callbacks
    if (s_SetCursorCallback) {
      if (DetourDetachSafe(s_SetCursorPosOrig, s_LocalSetCursorPos) != NO_ERROR) {
        auto msg = L"[MAGOS] ERROR: Detour Detach SetCursorPos"; 
        std::cout << kanan::narrow(msg) << std::endl;
        std::fflush(stdout);
        DXTRACE_MSG(msg);
      }
      s_SetCursorCallback = nullptr;
    }
    if (s_WndProcCallback && s_WndProcOrig && m_curHwnd) {
      SetWindowLongA(m_curHwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_WndProcOrig));
      s_WndProcCallback = nullptr;
    }
  };
 
  void SetupImGuiW32(WndProcCallbackT wndProcCallback, HWND hWnd = NULL)
  {
    std::call_once(m_ImGui_once, [this, wndProcCallback, hWnd]
    {
      // If unknown HWND, search for the current active one ...
      HWND lhWnd = hWnd;
      if (!lhWnd) {
        DWORD pid  = 0;
        DWORD cpid = GetCurrentProcessId();
        while (true) {
          lhWnd = FindWindowEx(NULL, lhWnd, NULL, NULL);
          if (lhWnd) {
            GetWindowThreadProcessId(lhWnd, &pid);
            if (pid == cpid) {
              break;
            }
          }
        }
      }

      // Override root WndProc and save original
      s_WndProcCallback = wndProcCallback;
      s_WndProcOrig     = reinterpret_cast<WNDPROC>(SetWindowLongA(lhWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_ThreadWndProc)));

      // SetCursorPos Hook
      s_SetCursorPosOrig  = SetCursorPos;
      s_SetCursorCallback = [this] (int X, int Y) {
        if (isDiagWindowFocused) {
          return TRUE;
        }
        return s_SetCursorPosOrig(X, Y);
      };
      if (DetourAttachSafe(s_SetCursorPosOrig, s_LocalSetCursorPos) != NO_ERROR) {
        auto msg = L"[MAGOS] ERROR: Detour Attach SetCursorPos"; 
        std::cout << kanan::narrow(msg) << std::endl;
        std::fflush(stdout);
        DXTRACE_MSG(msg);
      }

      // Setup ImGui context and Win32 impl.
      ImGui::CreateContext();

      ImGuiIO& io = ImGui::GetIO(); (void) io;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

      ImGui::StyleColorsDark();
      assert(ImGui_ImplWin32_Init(lhWnd));

      m_curHwnd = lhWnd;
    });
  };
  
  // Note: This will need to be called more than once if the game
  // creates a new device for a certain resolution. Leaving for now.
  bool SetupImGuiDX9(IDirect3DDevice9 *device) {
    return ImGui_ImplDX9_Init(device); 
  }

  void ResetImGuiDX9() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    ImGui_ImplDX9_CreateDeviceObjects();
  }

  // Abstract Input Handlers
  virtual bool HandleFOVKey(UCHAR VkKey)        = 0;
  virtual bool HandleDiagKey(UCHAR VkKey)       = 0;
  virtual bool HandleTranslateKey(UCHAR VkKey)  = 0;
  virtual void HandleInputDisable(bool disable) = 0;

  // Abstract Class Grabbers
  virtual Camera          *GetCamera(bool force = false) = 0;
  virtual GlobalData      *GetGlobalData(bool force = false) = 0;
  virtual PCDeviceManager *GetDeviceManager(bool force = false) = 0;
  virtual PCStateManager  *GetStateManager(bool force = false) = 0;
  virtual PCRenderDevice  *GetRenderDevice(bool force = false) = 0;

public:
  D3DXMATRIX  view;
  D3DXMATRIX  projection;
  D3DXMATRIX  world;
  D3DXMATRIX  ident;

  D3DXVECTOR3 camVec = D3DXVECTOR3 {0.0f, 0.0f,  2.26f};  // z=~2.26 without world transform
  D3DXVECTOR3 posVec = D3DXVECTOR3 {0.0f, 1.0f, -90.0f};  // position where to look at (sometimes z=45.0)
  D3DXVECTOR3 axiVec = D3DXVECTOR3 {0.0f, 0.0f,   1.0f};  // positive z-axis is up

  std::atomic_uint16_t       sceneSetPosN; 
  std::atomic_bool           inScene;
  std::atomic_bool           renderReady;
  std::atomic_uint32_t       frameNumber;
  const std::atomic_uint32_t frameStartNumber;
  std::atomic_bool           isDiagWindowFocused;
  std::vector<std::pair<bool, D3DXMATRIX>> lastConstData;

protected:
  std::unique_ptr<GlobalData>      m_GD;
  std::unique_ptr<Camera>          m_Camera;
  std::unique_ptr<PCDeviceManager> m_DeviceManager;
  std::unique_ptr<PCStateManager>  m_StateManager;
  std::unique_ptr<PCRenderDevice>  m_RenderDevice;
  
  std::atomic<float> m_currentCameraFOV;

  const char     *m_PCDeviceManagerSig;
  const uint16_t  m_PCDeviceManagerECXPtrOff;
  const uint16_t  m_PCDeviceManagerStateManagerOff;
  const uintptr_t m_CameraAddr;
  const uintptr_t m_GlobalDataAddr;
  const uintptr_t m_PCRenderDeviceAddr;
  const uintptr_t m_InputDisableAddr;

private:
  HWND m_curHwnd;
  std::once_flag  m_ImGui_once;
};
} // namespace skel::cdc
