#pragma once
#include <assert.h>
#include <d3dx9.h>

#include <memory>

#include "remixapi/bridge_remix_api.h"

#include "dxerror.hpp"
#include "statecb.hpp"
#include "device.hpp"
#include "meshloader.hpp"
#include "app_remix.hpp"

#ifndef DEFAULT_VIEW_DISTANCE
#define DEFAULT_VIEW_DISTANCE 1000.0f
#endif

class StateLock;
class AppCtx;
class AppCtxManager;

static bool g_bThreadSafe;
static CRITICAL_SECTION g_cs;
static std::unique_ptr<AppCtx> g_ctx;

// Pauses time or rendering.  Keeps a ref count so pausing can be layered
void WINAPI AppPause(bool bPauseTime, bool bPauseRendering);

// Return and optionally initialize the application global state 
AppCtx* GetAppCtx();

//--------------------------------------------------------------------------------------
// Helper macros to build member functions that access member variables with thread safety
//--------------------------------------------------------------------------------------
#define SET_ACCESSOR( x, y )       inline void Set##y( x t )   { _StateLock l; m_ctx.m_##y = t; };
#define GET_ACCESSOR( x, y )       inline x Get##y()           { _StateLock l; return m_ctx.m_##y; };
#define GET_SET_ACCESSOR( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR( x, y )

#define SETP_ACCESSOR( x, y )      inline void Set##y( x* t )  { _StateLock l; m_ctx.m_##y = *t; };
#define GETP_ACCESSOR( x, y )      inline x* Get##y()          { _StateLock l; return &m_ctx.m_##y; };
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
// Call-Once AppCtxManager
//--------------------------------------------------------------------------------------
class AppCtxManager {
public:
  AppCtxManager();
  ~AppCtxManager();
};


//--------------------------------------------------------------------------------------
// Thread-Safe (capable) App State Property Holder
//--------------------------------------------------------------------------------------
class AppCtx
{
protected:
  struct AppDeviceSettings
  {
    UINT       AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DFORMAT  AdapterFormat;
    DWORD      BehaviorFlags;
    D3DPRESENT_PARAMETERS pp;
  };
  struct Context {
    // State Properties
    // NOTE: Check property accessor macros in public section
    IDirect3D9*        m_D3D9;                   // the main D3D9 object
    IDirect3DDevice9*  m_D3D9Device;             // the D3D9 rendering device
    AppDeviceSettings* m_CurrentDeviceSettings;  // current device settings
    D3DSURFACE_DESC    m_BackBufferSurfaceDesc9; // D3D9 back buffer surface description
    D3DCAPS9 m_Caps;                             // D3D caps for current device
    bool     m_D3D9Inited;
    bool     m_DeviceLost;

    // RTX Remix Interface
    AppRemixInterface *m_Remix;

    // Device Enumerator
    CD3D9Enumeration *m_Enumerator;

    // Wavefront (.OBJ) Manager
    CMeshLoader *m_MeshLoader;
    
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

    WCHAR*    m_WindowTitle;
    HINSTANCE m_HInstance;
    HWND m_HWNDFocus;
    HWND m_HWNDDeviceWindowed;
    HWND m_HWNDDeviceFullScreen;

    double m_Time;          // current time in seconds
    double m_AbsoluteTime;  // absolute time in seconds
    float  m_ElapsedTime;   // time elapsed since last frame

    float m_MinViewDistance;
    float m_MaxViewDistance;
    
    float m_CameraFOV;
    D3DXVECTOR3 m_CameraVec;

    D3DXMATRIX  m_CurrentViewMatrix;
    D3DXMATRIX  m_CurrentProjectionMatrix;

    // State Callbacks
    LPAPPCTXCALLBACKISD3D9DEVICEACCEPTABLE m_IsD3D9DeviceAcceptableFunc; // D3D9 is device acceptable callback
    void* m_IsD3D9DeviceAcceptableFuncUserContext;                         // user context for is D3D9 device acceptable callback

    LPAPPCTXCALLBACKD3D9DEVICECREATED m_D3D9DeviceCreatedFunc;       // D3D9 device created callback
    void* m_D3D9DeviceCreatedFuncUserContext;                          // user context for D3D9 device created callback

    LPAPPCTXCALLBACKD3D9DEVICERESET m_D3D9DeviceResetFunc;           // D3D9 device reset callback
    void* m_D3D9DeviceResetFuncUserContext;                            // user context for D3D9 device reset callback

    LPAPPCTXCALLBACKD3D9DEVICELOST m_D3D9DeviceLostFunc;             // D3D9 device lost callback
    void* m_D3D9DeviceLostFuncUserContext;                             // user context for D3D9 device lost callback

    LPAPPCTXCALLBACKD3D9DEVICEDESTROYED m_D3D9DeviceDestroyedFunc;   // D3D9 device destroyed callback
    void* m_D3D9DeviceDestroyedFuncUserContext;                        // user context for D3D9 device destroyed callback

    LPAPPCTXCALLBACKD3D9FRAMERENDER m_D3D9FrameRenderFunc;           // D3D9 frame render callback
    void* m_D3D9FrameRenderFuncUserContext;                            // user context for D3D9 frame render callback

    LPAPPCTXCALLBACKMODIFYDEVICESETTINGS m_ModifyDeviceSettingsFunc; // modify Direct3D device settings callback
    void* m_ModifyDeviceSettingsFuncUserContext;                       // user context for modify Direct3D device settings callback

    LPAPPCTXCALLBACKDEVICEREMOVED m_DeviceRemovedFunc;               // Direct3D device removed callback
    void* m_DeviceRemovedFuncUserContext;                              // user context for Direct3D device removed callback

    LPAPPCTXCALLBACKFRAMEMOVE m_FrameMoveFunc;       // frame move callback
    void* m_FrameMoveFuncUserContext;                  // user context for frame move callback

    LPAPPCTXCALLBACKKEYBOARD m_KeyboardFunc;         // keyboard callback
    void* m_KeyboardFuncUserContext;                   // user context for keyboard callback

    LPAPPCTXCALLBACKMOUSE m_MouseFunc;               // mouse callback
    void* m_MouseFuncUserContext;                      // user context for mouse callback

    LPAPPCTXCALLBACKMSGPROC m_WindowMsgFunc;         // window messages callback   
    void* m_WindowMsgFuncUserContext;                  // user context for window messages callback
  };

  Context m_ctx;

public:
  AppCtx() {
    assert(!g_ctx);
    if (g_bThreadSafe) {
      assert(InitializeCriticalSectionAndSpinCount(&g_cs, 1000));
    }
    ZeroMemory(&m_ctx, sizeof(Context));
    ZeroMemory(&m_ctx.m_CameraVec, sizeof(m_ctx.m_CameraVec));

    // Defaults
    m_ctx.m_OverrideStartX  = -1;
    m_ctx.m_OverrideStartY  = -1;

    m_ctx.m_IsActive        = true;

    m_ctx.m_HandleEscape    = true;
    m_ctx.m_HandleAltEnter  = true;
    m_ctx.m_HandlePause     = true;

    m_ctx.m_MinViewDistance = -(DEFAULT_VIEW_DISTANCE);
    m_ctx.m_MaxViewDistance =   DEFAULT_VIEW_DISTANCE;

    m_ctx.m_CameraVec.x = 0;
    m_ctx.m_CameraVec.y = 0;
    m_ctx.m_CameraVec.z = -20.0f;

    m_ctx.m_CameraFOV = 60.0f;
    m_ctx.m_CurrentViewMatrix       = D3DXMATRIX();
    m_ctx.m_CurrentProjectionMatrix = D3DXMATRIX();
  }
  ~AppCtx() {
    if (g_bThreadSafe) {
      DeleteCriticalSection(&g_cs);
    }
    delete m_ctx.m_Enumerator;
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
  GET_SET_ACCESSOR   (bool, D3D9Inited);
  GET_SET_ACCESSOR   (bool, DeviceLost);
  
  // RTX Remix
  GET_SET_ACCESSOR(AppRemixInterface*, Remix);
 
  // Device Enumerator
  GET_SET_ACCESSOR(CD3D9Enumeration*, Enumerator);

  // Wavefront (.OBJ) Manager
  GET_SET_ACCESSOR(CMeshLoader*, MeshLoader);

  // Inputs
  GET_ACCESSOR(bool*,  Keys);
  GET_ACCESSOR(bool*,  LastKeys);
  GET_ACCESSOR(bool*,  MouseButtons);

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
  GET_SET_ACCESSOR(WCHAR*, WindowTitle);
  GET_SET_ACCESSOR(HINSTANCE, HInstance);
  GET_SET_ACCESSOR(HWND, HWNDFocus);
  GET_SET_ACCESSOR(HWND, HWNDDeviceWindowed);
  GET_SET_ACCESSOR(HWND, HWNDDeviceFullScreen);
  GET_SET_ACCESSOR(bool, ShowCursorWhenFullScreen);

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
  GET_SET_ACCESSOR(float, CameraFOV);

  // View Transforms
  GET_SET_ACCESSOR(D3DXMATRIX, CurrentViewMatrix);
  GET_SET_ACCESSOR(D3DXMATRIX, CurrentProjectionMatrix);
  
  //--------------------------------------------------------------------------------------
  // Callback Property Accessors
  //--------------------------------------------------------------------------------------
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKISD3D9DEVICEACCEPTABLE, IsD3D9DeviceAcceptableFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKD3D9DEVICECREATED,      D3D9DeviceCreatedFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKD3D9DEVICERESET,        D3D9DeviceResetFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKD3D9DEVICELOST,         D3D9DeviceLostFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKD3D9DEVICEDESTROYED,    D3D9DeviceDestroyedFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKD3D9FRAMERENDER,        D3D9FrameRenderFunc);

  GET_SET_ACCESSOR(LPAPPCTXCALLBACKMODIFYDEVICESETTINGS, ModifyDeviceSettingsFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKDEVICEREMOVED,        DeviceRemovedFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKFRAMEMOVE,            FrameMoveFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKKEYBOARD,             KeyboardFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKMOUSE,                MouseFunc);
  GET_SET_ACCESSOR(LPAPPCTXCALLBACKMSGPROC,              WindowMsgFunc);


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
  void WINAPI AppSetCallbackD3D9DeviceAcceptable(LPAPPCTXCALLBACKISD3D9DEVICEACCEPTABLE pCallback, void* pUserContext)
  {
    GetAppCtx()->SetIsD3D9DeviceAcceptableFunc(pCallback);
    GetAppCtx()->SetIsD3D9DeviceAcceptableFuncUserContext(pUserContext);
  };
  void WINAPI AppSetCallbackD3D9DeviceCreated(LPAPPCTXCALLBACKD3D9DEVICECREATED pCallback, void* pUserContext)
  {
    GetAppCtx()->SetD3D9DeviceCreatedFunc(pCallback);
    GetAppCtx()->SetD3D9DeviceCreatedFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackD3D9DeviceReset(LPAPPCTXCALLBACKD3D9DEVICERESET pCallback, void* pUserContext)
  {
    GetAppCtx()->SetD3D9DeviceResetFunc(pCallback);
    GetAppCtx()->SetD3D9DeviceResetFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackD3D9DeviceLost(LPAPPCTXCALLBACKD3D9DEVICELOST pCallback, void* pUserContext)
  {
    GetAppCtx()->SetD3D9DeviceLostFunc(pCallback);
    GetAppCtx()->SetD3D9DeviceLostFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackD3D9DeviceDestroyed(LPAPPCTXCALLBACKD3D9DEVICEDESTROYED pCallback, void* pUserContext)
  {
    GetAppCtx()->SetD3D9DeviceDestroyedFunc(pCallback);
    GetAppCtx()->SetD3D9DeviceDestroyedFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackD3D9FrameRender(LPAPPCTXCALLBACKD3D9FRAMERENDER pCallback, void* pUserContext)
  {
    GetAppCtx()->SetD3D9FrameRenderFunc(pCallback);
    GetAppCtx()->SetD3D9FrameRenderFuncUserContext(pUserContext);
  }
  void WINAPI AppGetCallbackD3D9DeviceAcceptable(LPAPPCTXCALLBACKISD3D9DEVICEACCEPTABLE* ppCallback, void** ppUserContext)
  {
    *ppCallback    = GetAppCtx()->GetIsD3D9DeviceAcceptableFunc();
    *ppUserContext = GetAppCtx()->GetIsD3D9DeviceAcceptableFuncUserContext();
  }
  
  //--------------------------------------------------------------------------------------
  // Callback Setters
  // Optional
  //--------------------------------------------------------------------------------------
  void WINAPI AppSetCallbackDeviceChanging(LPAPPCTXCALLBACKMODIFYDEVICESETTINGS pCallback, void *pUserContext) {
    GetAppCtx()->SetModifyDeviceSettingsFunc(pCallback);
    GetAppCtx()->SetModifyDeviceSettingsFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackDeviceRemoved(LPAPPCTXCALLBACKDEVICEREMOVED pCallback, void *pUserContext) {
    GetAppCtx()->SetDeviceRemovedFunc(pCallback);
    GetAppCtx()->SetDeviceRemovedFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackFrameMove(LPAPPCTXCALLBACKFRAMEMOVE pCallback, void *pUserContext) {
    GetAppCtx()->SetFrameMoveFunc(pCallback);
    GetAppCtx()->SetFrameMoveFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackKeyboard(LPAPPCTXCALLBACKKEYBOARD pCallback, void *pUserContext) {
    GetAppCtx()->SetKeyboardFunc(pCallback);
    GetAppCtx()->SetKeyboardFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackMouse(LPAPPCTXCALLBACKMOUSE pCallback, bool bIncludeMouseMove, void *pUserContext) {
    GetAppCtx()->SetMouseFunc(pCallback);
    GetAppCtx()->SetNotifyOnMouseMove(bIncludeMouseMove);
    GetAppCtx()->SetMouseFuncUserContext(pUserContext);
  }
  void WINAPI AppSetCallbackMsgProc(LPAPPCTXCALLBACKMSGPROC pCallback, void *pUserContext) {
    GetAppCtx()->SetWindowMsgFunc(pCallback);
    GetAppCtx()->SetWindowMsgFuncUserContext(pUserContext);
  }
};

