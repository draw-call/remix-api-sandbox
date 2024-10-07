#pragma once

#include <assert.h>
#include <d3dx9.h>

#include <memory>

#include "remixapi/bridge_remix_api.h"

#include "dxerror.hpp"
#include "statecb.hpp"
#include "device.hpp"

class StateLock;
class AppState;
class AppStateManager;

static std::unique_ptr<AppState> g_state;

static CRITICAL_SECTION g_cs;
static bool             g_bThreadSafe;

// Pauses time or rendering.  Keeps a ref count so pausing can be layered
void WINAPI AppPause(bool bPauseTime, bool bPauseRendering);

// Return and optionally initialize the application global state 
AppState* GetAppState();

//--------------------------------------------------------------------------------------
// Helper macros to build member functions that access member variables with thread safety
//--------------------------------------------------------------------------------------
#define SET_ACCESSOR( x, y )       inline void Set##y( x t )   { _StateLock l; m_state.m_##y = t; };
#define GET_ACCESSOR( x, y )       inline x Get##y()           { _StateLock l; return m_state.m_##y; };
#define GET_SET_ACCESSOR( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR( x, y )

#define SETP_ACCESSOR( x, y )      inline void Set##y( x* t )  { _StateLock l; m_state.m_##y = *t; };
#define GETP_ACCESSOR( x, y )      inline x* Get##y()          { _StateLock l; return &m_state.m_##y; };
#define GETP_SETP_ACCESSOR( x, y ) SETP_ACCESSOR( x, y ) GETP_ACCESSOR( x, y )


//--------------------------------------------------------------------------------------
// Automatically enters & leaves the CS upon object creation/deletion
//--------------------------------------------------------------------------------------
class _StateLock {
public:
  inline _StateLock()
  {
    if (g_bThreadSafe)
      EnterCriticalSection(&g_cs);
  }
  inline ~_StateLock()
  {
    if (g_bThreadSafe)
      LeaveCriticalSection(&g_cs);
  }
};

//--------------------------------------------------------------------------------------
// Call-Once AppStateManager
//--------------------------------------------------------------------------------------
class AppStateManager {
public:
  AppStateManager();
  ~AppStateManager();
};


//--------------------------------------------------------------------------------------
// Thread-Safe (capable) App State Property Holder
//--------------------------------------------------------------------------------------
class AppState
{
protected:
  struct AppDeviceSettings
  {
    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DFORMAT AdapterFormat;
    DWORD BehaviorFlags;
    D3DPRESENT_PARAMETERS pp;
  };
  struct State {
    // State Properties
    // NOTE: Check property accessor macros in public section
    IDirect3D9* m_D3D9;                         // the main D3D9 object
    IDirect3DDevice9* m_D3D9Device;             // the D3D9 rendering device
    AppDeviceSettings* m_CurrentDeviceSettings; // current device settings
    D3DSURFACE_DESC m_BackBufferSurfaceDesc9;   // D3D9 back buffer surface description
    D3DCAPS9 m_Caps;                            // D3D caps for current device
    bool m_DeviceLost;

    // RTX Remix Interface
    remixapi_Interface m_Remix;

    // Device Enumerator
    CD3D9Enumeration *m_Enumerator;
    
    bool m_Keys[256];       // array of key state
    bool m_LastKeys[256];   // array of last key state
    bool m_MouseButtons[5]; // array of mouse states

    int  m_OverrideStartX;   // if != -1, then override to this X position of the window
    int  m_OverrideStartY;   // if != -1, then override to this Y position of the window
    int  m_OverrideWidth;    // if != 0, then override to this width
    int  m_OverrideHeight;   // if != 0, then override to this height
    bool m_WindowCreatedWithDefaultPositions; // if true, then CW_USEDEFAULT was used and the window should be moved to the right adapter
    
    bool m_Inited;
    bool m_InitCalled;
    bool m_WindowCreateCalled;
    bool m_DeviceCreateCalled;
    bool m_DeviceObjectsCreated; // if true, then DeviceCreated callback has been called (if non-NULL)
    bool m_DeviceObjectsReset;   // if true, then DeviceReset callback has been called (if non-NULL)
    
    bool m_InsideDeviceCallback; // if true, then the framework is inside an app device callback
    bool m_InsideMainloop;       // if true, then the framework is inside the main loop
    bool m_IsActive;
    
    bool m_IsRenderingPaused;
    bool m_IsTimePaused;
    
    bool m_IsWindowed;
    bool m_IsFullscreen;
    bool m_ShowCursorWhenFullScreen;

    int  m_PauseRenderingCount;
    int  m_PauseTimeCount;

    bool m_WindowCreated;
    bool m_DeviceCreated;

    bool m_HandleEscape;    // if true, then app will handle escape to quit
    bool m_HandleAltEnter;  // if true, then app will handle alt-enter to toggle fullscreen
    bool m_HandlePause;     // if true, then app will handle pause to toggle time pausing

    bool m_InSizeMove;
    bool m_Minimized;
    bool m_Maximized;
    bool m_NotifyOnMouseMove;  // if true, include WM_MOUSEMOVE in mousecallback

    HINSTANCE m_HInstance;
    HWND  m_HWNDFocus;
    HWND  m_HWNDDeviceWindowed;
    HWND  m_HWNDDeviceFullScreen;
    WCHAR* m_WindowTitle;

    double m_Time;          // current time in seconds
    double m_AbsoluteTime;  // absolute time in seconds
    float  m_ElapsedTime;   // time elapsed since last frame

    float  m_MinViewDistance;
    float  m_MaxViewDistance;

    D3DXVECTOR3  m_CameraVec;
    D3DXMATRIX  m_CurrentViewMatrix;
    D3DXMATRIX  m_CurrentProjectionMatrix;

    // State Callbacks
    LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE m_IsD3D9DeviceAcceptableFunc; // D3D9 is device acceptable callback
    void* m_IsD3D9DeviceAcceptableFuncUserContext;                         // user context for is D3D9 device acceptable callback

    LPAPPSTATECALLBACKD3D9DEVICECREATED m_D3D9DeviceCreatedFunc;       // D3D9 device created callback
    void* m_D3D9DeviceCreatedFuncUserContext;                          // user context for D3D9 device created callback

    LPAPPSTATECALLBACKD3D9DEVICERESET m_D3D9DeviceResetFunc;           // D3D9 device reset callback
    void* m_D3D9DeviceResetFuncUserContext;                            // user context for D3D9 device reset callback

    LPAPPSTATECALLBACKD3D9DEVICELOST m_D3D9DeviceLostFunc;             // D3D9 device lost callback
    void* m_D3D9DeviceLostFuncUserContext;                             // user context for D3D9 device lost callback

    LPAPPSTATECALLBACKD3D9DEVICEDESTROYED m_D3D9DeviceDestroyedFunc;   // D3D9 device destroyed callback
    void* m_D3D9DeviceDestroyedFuncUserContext;                        // user context for D3D9 device destroyed callback

    LPAPPSTATECALLBACKD3D9FRAMERENDER m_D3D9FrameRenderFunc;           // D3D9 frame render callback
    void* m_D3D9FrameRenderFuncUserContext;                            // user context for D3D9 frame render callback

    LPAPPSTATECALLBACKMODIFYDEVICESETTINGS m_ModifyDeviceSettingsFunc; // modify Direct3D device settings callback
    void* m_ModifyDeviceSettingsFuncUserContext;                       // user context for modify Direct3D device settings callback

    LPAPPSTATECALLBACKDEVICEREMOVED m_DeviceRemovedFunc;               // Direct3D device removed callback
    void* m_DeviceRemovedFuncUserContext;                              // user context for Direct3D device removed callback

    LPAPPSTATECALLBACKFRAMEMOVE m_FrameMoveFunc;       // frame move callback
    void* m_FrameMoveFuncUserContext;                  // user context for frame move callback

    LPAPPSTATECALLBACKKEYBOARD m_KeyboardFunc;         // keyboard callback
    void* m_KeyboardFuncUserContext;                   // user context for keyboard callback

    LPAPPSTATECALLBACKMOUSE m_MouseFunc;               // mouse callback
    void* m_MouseFuncUserContext;                      // user context for mouse callback

    LPAPPSTATECALLBACKMSGPROC m_WindowMsgFunc;         // window messages callback   
    void* m_WindowMsgFuncUserContext;                  // user context for window messages callback
  };

  State m_state;

public:
  AppState() {
    assert(!g_state);
    if (g_bThreadSafe) {
      InitializeCriticalSectionAndSpinCount(&g_cs, 1000);
    }
    ZeroMemory(&m_state, sizeof(State));

    // Defaults
    m_state.m_OverrideStartX  = -1;
    m_state.m_OverrideStartY  = -1;

    m_state.m_IsActive        = true;

    m_state.m_HandleEscape    = true;
    m_state.m_HandleAltEnter  = true;
    m_state.m_HandlePause     = true;

    m_state.m_MinViewDistance = -1000.0f;
    m_state.m_MaxViewDistance =  1000.0f;

    ZeroMemory(&m_state.m_CameraVec, sizeof(m_state.m_CameraVec));
    m_state.m_CameraVec.x = 0;
    m_state.m_CameraVec.y = 0;
    m_state.m_CameraVec.z = -20.0f;

    m_state.m_CurrentViewMatrix       = D3DXMATRIX();
    m_state.m_CurrentProjectionMatrix = D3DXMATRIX();
  }
  ~AppState() {
    if (g_bThreadSafe) {
      DeleteCriticalSection(&g_cs);
    }
    delete m_state.m_Enumerator;
  }

  //--------------------------------------------------------------------------------------
  // Property Accessors
  //--------------------------------------------------------------------------------------

  // D3D9
  GET_SET_ACCESSOR   (IDirect3D9*, D3D9);
  GET_SET_ACCESSOR   (IDirect3DDevice9*, D3D9Device);
  GET_SET_ACCESSOR   (AppDeviceSettings*, CurrentDeviceSettings);
  GETP_SETP_ACCESSOR (D3DSURFACE_DESC, BackBufferSurfaceDesc9);
  GETP_SETP_ACCESSOR (D3DCAPS9, Caps);
  GET_SET_ACCESSOR   (bool, DeviceLost);
  
  // RTX Remix
  GET_SET_ACCESSOR(remixapi_Interface, Remix);
 
  // Device Enumerator
  GET_SET_ACCESSOR(CD3D9Enumeration*, Enumerator);

  // Inputs
  GET_ACCESSOR(bool*, Keys);
  GET_ACCESSOR(bool*, LastKeys);
  GET_ACCESSOR(bool*, MouseButtons);

  // Window Overrides
  GET_SET_ACCESSOR(int,  OverrideStartX);
  GET_SET_ACCESSOR(int,  OverrideStartY);
  GET_SET_ACCESSOR(int,  OverrideWidth);
  GET_SET_ACCESSOR(int,  OverrideHeight);
  GET_SET_ACCESSOR(bool, WindowCreatedWithDefaultPositions);


  // Window Movement
  GET_SET_ACCESSOR(bool, InSizeMove);
  GET_SET_ACCESSOR(bool, Minimized);
  GET_SET_ACCESSOR(bool, Maximized);

  // Window Management
  GET_SET_ACCESSOR(HINSTANCE, HInstance);
  GET_SET_ACCESSOR(HWND, HWNDFocus);
  GET_SET_ACCESSOR(HWND, HWNDDeviceWindowed);
  GET_SET_ACCESSOR(HWND, HWNDDeviceFullScreen);
  GET_SET_ACCESSOR(bool, ShowCursorWhenFullScreen);
  GET_SET_ACCESSOR(WCHAR *, WindowTitle);

  // Init Flags
  GET_SET_ACCESSOR(bool, Inited);
  GET_SET_ACCESSOR(bool, InitCalled);
  GET_SET_ACCESSOR(bool, IsActive);
  GET_SET_ACCESSOR(bool, IsRenderingPaused);
  GET_SET_ACCESSOR(bool, IsTimePaused);
  GET_SET_ACCESSOR(bool, IsWindowed);
  GET_SET_ACCESSOR(bool, IsFullscreen);
  
  // Creation Notificaitons
  GET_SET_ACCESSOR(bool, WindowCreateCalled);
  GET_SET_ACCESSOR(bool, DeviceCreateCalled);
  GET_SET_ACCESSOR(bool, WindowCreated);
  GET_SET_ACCESSOR(bool, DeviceCreated);

  // Internal Notifications
  GET_SET_ACCESSOR(bool, NotifyOnMouseMove);
  GET_SET_ACCESSOR(bool, InsideDeviceCallback);
  GET_SET_ACCESSOR(bool, InsideMainloop);
  GET_SET_ACCESSOR(bool, DeviceObjectsCreated);
  GET_SET_ACCESSOR(bool, DeviceObjectsReset);

  // Render Pause
  GET_SET_ACCESSOR(int, PauseRenderingCount);
  GET_SET_ACCESSOR(int, PauseTimeCount);
  
  // Key Handle Notifications
  GET_SET_ACCESSOR(bool, HandleEscape);
  GET_SET_ACCESSOR(bool, HandleAltEnter);
  GET_SET_ACCESSOR(bool, HandlePause);

  // Timer
  GET_SET_ACCESSOR(double, Time);
  GET_SET_ACCESSOR(double, AbsoluteTime);
  GET_SET_ACCESSOR(float,  ElapsedTime);

  // View Transform Info
  GET_SET_ACCESSOR(float,  MinViewDistance);
  GET_SET_ACCESSOR(float,  MaxViewDistance);
  GET_SET_ACCESSOR(D3DXVECTOR3, CameraVec);

  // View Transforms
  GET_SET_ACCESSOR(D3DXMATRIX, CurrentViewMatrix);
  GET_SET_ACCESSOR(D3DXMATRIX, CurrentProjectionMatrix);
  
  //--------------------------------------------------------------------------------------
  // Callback Property Accessors
  //--------------------------------------------------------------------------------------
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE, IsD3D9DeviceAcceptableFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKD3D9DEVICECREATED,      D3D9DeviceCreatedFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKD3D9DEVICERESET,        D3D9DeviceResetFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKD3D9DEVICELOST,         D3D9DeviceLostFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKD3D9DEVICEDESTROYED,    D3D9DeviceDestroyedFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKD3D9FRAMERENDER,        D3D9FrameRenderFunc);

  GET_SET_ACCESSOR(LPAPPSTATECALLBACKMODIFYDEVICESETTINGS, ModifyDeviceSettingsFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKDEVICEREMOVED,        DeviceRemovedFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKFRAMEMOVE,            FrameMoveFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKKEYBOARD,             KeyboardFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKMOUSE,                MouseFunc);
  GET_SET_ACCESSOR(LPAPPSTATECALLBACKMSGPROC,              WindowMsgFunc);


  //--------------------------------------------------------------------------------------
  // Callback Context Accessors
  //--------------------------------------------------------------------------------------
  GET_SET_ACCESSOR(void*, IsD3D9DeviceAcceptableFuncUserContext);
  GET_SET_ACCESSOR(void*, D3D9DeviceCreatedFuncUserContext);
  GET_SET_ACCESSOR(void*, D3D9DeviceResetFuncUserContext);
  GET_SET_ACCESSOR(void*, D3D9DeviceLostFuncUserContext);
  GET_SET_ACCESSOR(void*, D3D9DeviceDestroyedFuncUserContext);
  GET_SET_ACCESSOR(void*, D3D9FrameRenderFuncUserContext);


  GET_SET_ACCESSOR(void*, ModifyDeviceSettingsFuncUserContext);
  GET_SET_ACCESSOR(void*, DeviceRemovedFuncUserContext);
  GET_SET_ACCESSOR(void*, FrameMoveFuncUserContext);
  GET_SET_ACCESSOR(void*, KeyboardFuncUserContext);
  GET_SET_ACCESSOR(void*, MouseFuncUserContext);
  GET_SET_ACCESSOR(void*, WindowMsgFuncUserContext);

  //--------------------------------------------------------------------------------------
  // Callback Setters
  // Mandatory
  //--------------------------------------------------------------------------------------
  void WINAPI AppSetCallbackD3D9DeviceAcceptable(LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE pCallback, void* pUserContext)
  {
    GetAppState()->SetIsD3D9DeviceAcceptableFunc(pCallback);
    GetAppState()->SetIsD3D9DeviceAcceptableFuncUserContext(pUserContext);
  };
  void WINAPI AppSetCallbackD3D9DeviceCreated(LPAPPSTATECALLBACKD3D9DEVICECREATED pCallback, void* pUserContext)
  {
    GetAppState()->SetD3D9DeviceCreatedFunc(pCallback);
    GetAppState()->SetD3D9DeviceCreatedFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackD3D9DeviceReset(LPAPPSTATECALLBACKD3D9DEVICERESET pCallback, void* pUserContext)
  {
    GetAppState()->SetD3D9DeviceResetFunc(pCallback);
    GetAppState()->SetD3D9DeviceResetFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackD3D9DeviceLost(LPAPPSTATECALLBACKD3D9DEVICELOST pCallback, void* pUserContext)
  {
    GetAppState()->SetD3D9DeviceLostFunc(pCallback);
    GetAppState()->SetD3D9DeviceLostFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackD3D9DeviceDestroyed(LPAPPSTATECALLBACKD3D9DEVICEDESTROYED pCallback, void* pUserContext)
  {
    GetAppState()->SetD3D9DeviceDestroyedFunc(pCallback);
    GetAppState()->SetD3D9DeviceDestroyedFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackD3D9FrameRender(LPAPPSTATECALLBACKD3D9FRAMERENDER pCallback, void* pUserContext)
  {
    GetAppState()->SetD3D9FrameRenderFunc(pCallback);
    GetAppState()->SetD3D9FrameRenderFuncUserContext(pUserContext);
  }
  void WINAPI AppGetCallbackD3D9DeviceAcceptable(LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE* ppCallback, void** ppUserContext)
  {
    *ppCallback    = GetAppState()->GetIsD3D9DeviceAcceptableFunc();
    *ppUserContext = GetAppState()->GetIsD3D9DeviceAcceptableFuncUserContext();
  }
  
  //--------------------------------------------------------------------------------------
  // Callback Setters
  // Optional
  //--------------------------------------------------------------------------------------
  void WINAPI AppSetCallbackDeviceChanging(LPAPPSTATECALLBACKMODIFYDEVICESETTINGS pCallback, void *pUserContext) {
    GetAppState()->SetModifyDeviceSettingsFunc(pCallback);
    GetAppState()->SetModifyDeviceSettingsFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackDeviceRemoved(LPAPPSTATECALLBACKDEVICEREMOVED pCallback, void *pUserContext) {
    GetAppState()->SetDeviceRemovedFunc(pCallback);
    GetAppState()->SetDeviceRemovedFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackFrameMove(LPAPPSTATECALLBACKFRAMEMOVE pCallback, void *pUserContext) {
    GetAppState()->SetFrameMoveFunc(pCallback);
    GetAppState()->SetFrameMoveFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackKeyboard(LPAPPSTATECALLBACKKEYBOARD pCallback, void *pUserContext) {
    GetAppState()->SetKeyboardFunc(pCallback);
    GetAppState()->SetKeyboardFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackMouse(LPAPPSTATECALLBACKMOUSE pCallback, bool bIncludeMouseMove, void *pUserContext) {
    GetAppState()->SetMouseFunc(pCallback);
    GetAppState()->SetNotifyOnMouseMove(bIncludeMouseMove);
    GetAppState()->SetMouseFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackMsgProc(LPAPPSTATECALLBACKMSGPROC pCallback, void *pUserContext) {
    GetAppState()->SetWindowMsgFunc(pCallback);
    GetAppState()->SetWindowMsgFuncUserContext(pUserContext);
  }
};

