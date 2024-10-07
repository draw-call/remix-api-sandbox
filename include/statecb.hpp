#pragma once

#include <d3dx9.h>

// IsDeviceAcceptable
typedef bool(CALLBACK *LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE)(
    D3DCAPS9 *pCaps,
    D3DFORMAT AdapterFormat,
    D3DFORMAT BackBufferFormat,
    bool bWindowed,
    void *pUserContext
);

// DeviceCreated
typedef HRESULT(CALLBACK *LPAPPSTATECALLBACKD3D9DEVICECREATED)(
    IDirect3DDevice9 *pd3dDevice,
    const D3DSURFACE_DESC *pBackBufferSurfaceDesc,
    void *pUserContext
);

// DeviceDestroyed
typedef void(CALLBACK *LPAPPSTATECALLBACKD3D9DEVICEDESTROYED)(void *pUserContext);

// DeviceRemoved
typedef bool(CALLBACK *LPAPPSTATECALLBACKDEVICEREMOVED)(void* pUserContext);

// DeviceLost
typedef void(CALLBACK *LPAPPSTATECALLBACKD3D9DEVICELOST)(void *pUserContext);

// DeviceReset
typedef HRESULT(CALLBACK *LPAPPSTATECALLBACKD3D9DEVICERESET)(
    IDirect3DDevice9 *pd3dDevice,
    const D3DSURFACE_DESC *pBackBufferSurfaceDesc,
    void *pUserContext
);

// FrameRender
typedef void(CALLBACK *LPAPPSTATECALLBACKD3D9FRAMERENDER)(
    IDirect3DDevice9 *pd3dDevice,
    double fTime,
    float fElapsedTime,
    void *pUserContext
);

// FrameMove
typedef void(CALLBACK *LPAPPSTATECALLBACKFRAMEMOVE)(
  double fTime,
  float fElapsedTime,
  void *pUserContext
);

// KeyBoard Event
typedef void(CALLBACK *LPAPPSTATECALLBACKKEYBOARD)(
  UINT nChar,
  bool bKeyDown,
  bool bAltDown,
  void *pUserContext
);

// MouseEvent
typedef void(CALLBACK *LPAPPSTATECALLBACKMOUSE)(
  bool bLeftButtonDown,
  bool bRightButtonDown,
  bool bMiddleButtonDown,
  bool bSideButton1Down,
  bool bSideButton2Down,
  int  nMouseWheelDelta,
  int  xPos,
  int  yPos,
  void *pUserContext
);

// MsgProc
typedef LRESULT(CALLBACK *LPAPPSTATECALLBACKMSGPROC)(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam,
  bool *pbNoFurtherProcessing,
  void *pUserContext );

// ModifyDeviceSettings
typedef bool(CALLBACK *LPAPPSTATECALLBACKMODIFYDEVICESETTINGS)(
  void *pDeviceSettings,
  void *pUserContext
);


