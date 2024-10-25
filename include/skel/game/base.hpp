#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <iostream>

#include <d3dx9.h>

#include "kanan.h"
#include "dxerror.hpp"

#include "skel/cdc_internal.hpp"

namespace skel::cdc {

class GameStateBase {
public:
  GameStateBase(
    const char    *PCDeviceManagerSig,
    const uint16_t PCDeviceManagerECXPtrOff,
    const uint16_t PCDeviceManagerStateManagerOff,
    const uint32_t CameraAddr,
    const uint32_t GlobalDataAddr,
    const uint32_t PCRenderDeviceAddr
  ) :
    m_PCDeviceManagerSig(PCDeviceManagerSig),
    m_PCDeviceManagerECXPtrOff(PCDeviceManagerECXPtrOff),
    m_PCDeviceManagerStateManagerOff(PCDeviceManagerStateManagerOff),
    m_CameraAddr(CameraAddr),
    m_GlobalDataAddr(GlobalDataAddr),
    m_PCRenderDeviceAddr(PCRenderDeviceAddr)
  { };

  ~GameStateBase() { };

  virtual GlobalData *GetGlobalData(bool force = false) {
    if (m_GD == nullptr || force) {
      m_GD.reset(reinterpret_cast<GlobalData *>(m_GlobalDataAddr)); 
    }
    return m_GD.get();
  }

  virtual Camera *GetCamera(bool force = false) {
    if (m_Camera == nullptr || force) {
      m_Camera.reset(reinterpret_cast<Camera *>(m_CameraAddr));
    }
    return m_Camera.get();
  }

  virtual PCDeviceManager *GetDeviceManager(bool force = false) {
    if (m_DeviceManager == nullptr || force) {
      const auto PCDeviceManagerMatch = kanan::scan(m_PCDeviceManagerSig); 
      if (!PCDeviceManagerMatch.has_value()) {
        auto msg = L"[MAGOS] ERROR: cdc scan";
        std::cout << kanan::narrow(msg) << std::endl;
        DXTRACE_ERR_MSGBOX(msg, E_FAIL);
        abort();
      }

      const auto PCDeviceManagerPtrAddr = PCDeviceManagerMatch.value() + m_PCDeviceManagerECXPtrOff;
      if (!kanan::isGoodReadPtr(PCDeviceManagerPtrAddr, 4)) {
        auto msg = L"[MAGOS] ERROR: cdc readptr fail (too early?)";
        std::cout << kanan::narrow(msg) << std::endl;
        DXTRACE_ERR_MSGBOX(msg, E_FAIL);
        abort();
      }
      m_DeviceManager.reset(**(PCDeviceManager ***)(PCDeviceManagerPtrAddr));
    }
    return m_DeviceManager.get();
  }

  virtual PCStateManager *GetStateManager(bool force = false) {
#if defined(NEXTGEN_SHADER_MODE) || defined(USE_PCSTATEMANAGER)
    if (GetDeviceManager() && (!m_StateManager || force)) {
      const auto ptr = (m_DeviceManager.get() + m_PCDeviceManagerStateManagerOff);
      if (kanan::isGoodReadPtr(ptr, 4)) {
        m_StateManager.reset(*(reinterpret_cast<PCStateManager **>(ptr)));
      }
    }
#endif
    return m_StateManager.get();
  }

  virtual PCRenderDevice *GetRenderDevice(bool force = false) {
    if (m_RenderDevice == nullptr || force) {
      m_RenderDevice.reset(*(reinterpret_cast<PCRenderDevice **>(m_PCRenderDeviceAddr)));
    }
    return m_RenderDevice.get();
  }

  D3DXMATRIX  view;
  D3DXMATRIX  projection;
  D3DXMATRIX  world;

  D3DXVECTOR3 camVec = D3DXVECTOR3 {0.0f, 0.0f, 0.0f};  // z=~2.26 without world transform
  D3DXVECTOR3 posVec = D3DXVECTOR3 {0.0f, 1.0f, 0.0f};  // position where to look at (sometimes z=45.0)
  D3DXVECTOR3 axiVec = D3DXVECTOR3 {0.0f, 0.0f, 1.0f};  // positive z-axis is up 

private:

  std::unique_ptr<GlobalData>      m_GD;
  std::unique_ptr<Camera>          m_Camera;
  std::unique_ptr<PCDeviceManager> m_DeviceManager;
  std::unique_ptr<PCStateManager>  m_StateManager;
  std::unique_ptr<PCRenderDevice>  m_RenderDevice;
     
  const char    *m_PCDeviceManagerSig;
  const uint16_t m_PCDeviceManagerECXPtrOff;
  const uint16_t m_PCDeviceManagerStateManagerOff;
  const uint32_t m_CameraAddr;
  const uint32_t m_GlobalDataAddr;
  const uint32_t m_PCRenderDeviceAddr;
};

} // namespace skel::cdc
