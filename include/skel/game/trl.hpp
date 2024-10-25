#pragma once

#include "base.hpp"

constexpr auto *PCDeviceManagerSig = "A1 ? ? ? ? 8B 0D ? ? ? ? 68 ? ? ? ? 50 E8";

constexpr auto CameraAddr = 0x10FC660;
constexpr auto GlobalDataAddr = 0x1076980;
constexpr auto PCRenderDeviceAddr = 0x10027CC;

constexpr auto PCDeviceManagerECXPtrOff = 0x7;
constexpr auto PCDeviceManagerStateManagerOff = 0x214;

namespace skel::cdc {

class TRLState final : public GameStateBase {
  public:
    TRLState() : GameStateBase(
      PCDeviceManagerSig,
      PCDeviceManagerECXPtrOff,
      PCDeviceManagerStateManagerOff,
      CameraAddr,
      GlobalDataAddr,
      PCRenderDeviceAddr
    ) {};
    ~TRLState() {};

    void HandleTranslateKey(SHORT VkKey)
    {
      D3DXMATRIX  tranx;
      D3DXMATRIX  tran_rotx;
      D3DXMATRIX  trany;
      D3DXMATRIX  tran_roty;
      D3DXMATRIX  tranz;
      D3DXMATRIX  tran_rotz;
      D3DXVECTOR3 next {0, 0, 0};

      switch (VkKey)
      {
        // Rotation
        case 'U':
          D3DXMatrixRotationX(&tran_rotx,  0.0001);  
          view *= tran_rotx;
          break;
        case 'I':
          D3DXMatrixRotationY(&tran_roty,  0.0001);
          view *= tran_roty;
          break;
        case 'O':
          D3DXMatrixRotationZ(&tran_rotz,  0.0001);
          view *= tran_rotz;
          break;

        // Translation
        case 'J':
          next.z = 0.01;
          D3DXVec3Normalize(&next, &next);
          D3DXMatrixTranslation(&trany, next.x, next.y, next.z);
          view *= trany;
          break;
        case 'K':
          next.z = -0.01;
          D3DXVec3Normalize(&next, &next);
          D3DXMatrixTranslation(&trany, next.x, next.y, next.z);
          view *= trany;
          break;
        case 'L': 
          next.x = -0.01;
          D3DXVec3Normalize(&next, &next);
          D3DXMatrixTranslation(&tranx, next.x, next.y, next.z);
          view *= tranx;
          break;
        case 'H':
          next.x = 0.01;
          D3DXVec3Normalize(&next, &next);
          D3DXMatrixTranslation(&tranx, next.x, next.y, next.z);
          view *= tranx;
          break;

        // Z up / Z down
        case 'N':
          next.y = -0.1;
          D3DXVec3Normalize(&next, &next);
          D3DXMatrixTranslation(&tranz, next.x, next.y, next.z);
          view *= tranz;
          break;
        case 'M':
          next.y = 0.1;
          D3DXVec3Normalize(&next, &next);
          D3DXMatrixTranslation(&tranz, next.x, next.y, next.z);
          view *= tranz;
          break;

        default:
          break;
      }
    }
  };
}
