#pragma once
#include <atomic>
#include <algorithm>

#include <d3dx9.h>

#include "kanan.h"
#include "dxerror.hpp"

#include "skel/game/base.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace skel::cdc
{

HOOK_STDCALL(void, TRANS_SetTransformMatrixf, HARGS(cdc::Matrix* m)) {};

class TRLState final : public GameStateBase {
public:
   TRLState() : GameStateBase(), debugLight() { };
   ~TRLState() {};
    
  D3DLIGHT9 debugLight {};

  /*
   Root window message process override callback
  */
  void WndProcCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // For single-shot key-event handling (eg: ImGui Diag menu)
    // Note: d3d9ex.cpp handles a different types of keypress, such as multiple down per frame
    // without waiting for a message or getting the order incorrect.
    if (msg == WM_KEYUP) {
      this->HandleDiagKey(wParam);
    }
    if (this->isDiagWindowFocused) {
      ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    }
  }

  /*
   Transforms
  */
  inline void updateMatricies(void) {
#ifdef NEXTGEN_SHADER_MODE // nextgen shaders are too advanced, for now
    memcpy(world.m,      (float **)&GetStateManager()->m_worldMatrix,      sizeof(D3DXMATRIX));
    memcpy(view.m,       (float **)&GetStateManager()->m_viewMatrix,       sizeof(D3DXMATRIX));
    memcpy(projection.m, (float **)&GetStateManager()->m_projectionMatrix, sizeof(D3DXMATRIX));
#endif
    view  = reinterpret_cast<float *>(&GetCamera()->core.cwTransform2f);
    world = reinterpret_cast<float *>(&GetCamera()->core.wcTransform2f); // inverse of cwTransform
  }

  D3DXMATRIX CreateProjectionTransform(float width, float height, float fov = DefaultFovDeg, float minZ = DefaultMinDepth, float maxZ = DefaultMaxPath) {
    // Generic projection matrix
    D3DXMatrixPerspectiveFovRH(
      &projection,
      D3DXToRadian(fov),
      (width / height),
      minZ,
      maxZ
    );
    return projection;
  }

  /*
   Class Grabbers
  */
  GlobalData *GetGlobalData(bool force = false) {
    if (m_GD == nullptr || force) [[unlikely]] {
      if (!kanan::isGoodReadPtr(m_GlobalDataAddr, 4)) {
        auto msg = L"[MAGOS] ERROR: failed to find GlobalData";
        std::cout << kanan::narrow(msg) << std::endl;
        DXTRACE_ERR_MSGBOX(msg, E_FAIL);
        return nullptr;
      }
      m_GD.reset(reinterpret_cast<GlobalData *>(m_GlobalDataAddr)); 
    }
    return m_GD.get();
  }

  Camera *GetCamera(bool force = false) {
    if (m_Camera == nullptr || force) [[unlikely]] {
      if (!kanan::isGoodReadPtr(m_CameraAddr, 4)) {
        auto msg = L"[MAGOS] ERROR: failed to find Camera";
        std::cout << kanan::narrow(msg) << std::endl;
        DXTRACE_ERR_MSGBOX(msg, E_FAIL);
        return nullptr;
      }
      m_Camera.reset(reinterpret_cast<Camera *>(m_CameraAddr));
    }
    return m_Camera.get();
  }

  PCDeviceManager *GetDeviceManager(bool force = false) {
    if (m_DeviceManager == nullptr || force) [[unlikely]] {
      const auto PCDeviceManagerMatch = kanan::scan(m_PCDeviceManagerSig); 
      if (!PCDeviceManagerMatch.has_value()) {
        auto msg = L"[MAGOS] ERROR: CDC DeviceManager scan fail";
        std::cout << kanan::narrow(msg) << std::endl;
        DXTRACE_ERR_MSGBOX(msg, E_FAIL);
        return nullptr;
      }

      const auto PCDeviceManagerPtrAddr = PCDeviceManagerMatch.value() + m_PCDeviceManagerECXPtrOff;
      if (!kanan::isGoodReadPtr(PCDeviceManagerPtrAddr, 4)) {
        auto msg = L"[MAGOS] ERROR: failed to find DeviceManager";
        std::cout << kanan::narrow(msg) << std::endl;
        DXTRACE_ERR_MSGBOX(msg, E_FAIL);
        return nullptr;
      }
      m_DeviceManager.reset(**(PCDeviceManager ***)(PCDeviceManagerPtrAddr));
    }
    return m_DeviceManager.get();
  }

  PCStateManager *GetStateManager(bool force = false) {
#if defined(NEXTGEN_SHADER_MODE) || defined(USE_PCSTATEMANAGER)
    if (GetDeviceManager() && (m_StateManager == nullptr || force)) [[unlikely]] {
      const auto ptr = reinterpret_cast<uintptr_t>(m_DeviceManager.get() + m_PCDeviceManagerStateManagerOff);
      if (kanan::isGoodReadPtr(ptr, 4)) {
        m_StateManager.reset(*(reinterpret_cast<PCStateManager **>(ptr)));
      } else {
        return nullptr;
      }
    }
#endif
    return m_StateManager.get();
  }

  PCRenderDevice *GetRenderDevice(bool force = false) {
    if (m_RenderDevice == nullptr || force) [[unlikely]] {
      if (!kanan::isGoodReadPtr(m_PCRenderDeviceAddr, 4)) {
        auto msg = L"[MAGOS] ERROR: failed to find RenderDevice";
        std::cout << kanan::narrow(msg) << std::endl;
        DXTRACE_ERR_MSGBOX(msg, E_FAIL);
        return nullptr;
      }
      m_RenderDevice.reset(*(reinterpret_cast<PCRenderDevice **>(m_PCRenderDeviceAddr)));
    }
    return m_RenderDevice.get();
  }

  /*
   FOV Handlers
  */
  inline void CameraUpdateProjectionDistance(void) {
    GetCamera()->core.newProjDistance = m_currentCameraFOV;
    GetCamera()->core.projDistanceSpd = 20.0f;
  }

  inline void CameraUpdateProjectionDistance(cdc::Camera *camera) {
    camera->core.newProjDistance = m_currentCameraFOV;
    camera->core.projDistanceSpd = 20.0f;
  }

  inline void CameraSetProjectionDistance(cdc::Camera *camera, float fov) {
    camera->core.newProjDistance = fov;
    camera->core.projDistanceSpd = 20.0f;
  }
  void HandleInputDisable(bool disable) {
    *(bool*)(m_InputDisableAddr) = disable;
  }

  /*
   Input Handlers
  */
  bool HandleFOVKey(UCHAR VkKey) {
    switch (VkKey) {
      case VK_OEM_PLUS:
        m_currentCameraFOV = std::clamp(m_currentCameraFOV + 20.0f, 100.0f, 3000.0f);
        CameraSetProjectionDistance(GetCamera(), m_currentCameraFOV);
        return true;
      case VK_OEM_MINUS:
        m_currentCameraFOV = std::clamp(m_currentCameraFOV - 20.0f, 100.0f, 3000.0f);
        CameraSetProjectionDistance(GetCamera(), m_currentCameraFOV);
        return true;
      default:
        return false;
    }
  }

  bool HandleDiagKey(UCHAR VkKey) {
    switch (VkKey) {
      case VK_OEM_3:
        isDiagWindowFocused = !isDiagWindowFocused;
        // check if modifier was pressed,
        if ((GetAsyncKeyState(VK_SHIFT) & (1 << 15)) != 0) {
          ImGui::GetIO().MouseDrawCursor = false;
        } else {
          HandleInputDisable(isDiagWindowFocused);
          ImGui::GetIO().MouseDrawCursor = isDiagWindowFocused;
        }
        return true;
      default:
        return false;
    }
  }

  bool HandleTranslateKey(UCHAR VkKey) {
    D3DXMATRIX  tran;
    D3DXVECTOR3 vec {0, 0, 0};
    switch (VkKey)
    {
      // Rotation
      case 'U':
        D3DXMatrixRotationX(&tran,  0.0001);  
        view *= tran;
        return true;
      case 'I':
        D3DXMatrixRotationY(&tran,  0.0001);
        view *= tran;
        return true;
      case 'O':
        D3DXMatrixRotationZ(&tran,  0.0001);
        view *= tran;
        return true;

      // Translation
      case 'J':
        vec.z = 0.01;
        D3DXVec3Normalize(&vec, &vec);
        D3DXMatrixTranslation(&tran, vec.x, vec.y, vec.z);
        view *= tran;
        return true;
      case 'K':
        vec.z = -0.01;
        D3DXVec3Normalize(&vec, &vec);
        D3DXMatrixTranslation(&tran, vec.x, vec.y, vec.z);
        view *= tran;
        return true;
      case 'L': 
        vec.x = -0.01;
        D3DXVec3Normalize(&vec, &vec);
        D3DXMatrixTranslation(&tran, vec.x, vec.y, vec.z);
        view *= tran;
        return true;
      case 'H':
        vec.x = 0.01;
        D3DXVec3Normalize(&vec, &vec);
        D3DXMatrixTranslation(&tran, vec.x, vec.y, vec.z);
        view *= tran;
        return true;

      // Z up / Z down
      case 'N':
        vec.y = -0.1;
        D3DXVec3Normalize(&vec, &vec);
        D3DXMatrixTranslation(&tran, vec.x, vec.y, vec.z);
        view *= tran;
        return true;
      case 'M':
        vec.y = 0.1;
        D3DXVec3Normalize(&vec, &vec);
        D3DXMatrixTranslation(&tran, vec.x, vec.y, vec.z);
        view *= tran;
        return true;

      default:
        return false;
    }
  }
}; // class TRLState
}  // namespace skel::cdc
