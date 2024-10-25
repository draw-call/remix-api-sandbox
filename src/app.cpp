// D3D9 Remix Demo / Playground / Sandbox / Dogshit
// watbulb

#include <Windows.h>

#include <vector>
#include <memory>
#include <filesystem>

#include <d3dx9.h>
#include <d3d9types.h>

#include "app.hpp"

#ifndef AppMain
#define AppMain wWinMain
#endif

#ifndef AppName
#define AppName L"remix-api-sandbox"
#endif

// An ordinal exported routine to export devices to participating DLL's
EXTERN_C __declspec(dllexport) IDirect3DDevice9 *ExportDevice(IDirect3DDevice9 * device) {
  return device;
};

//-----------------------------------------------------------------------------
// Name: AppSetupD3DTransform()
// Desc: Setup initial view transforms
//-----------------------------------------------------------------------------
HRESULT AppSetupD3DTransform(
  HWND hWnd,
  IDirect3DDevice9 *pD3D9Device,
  D3DXMATRIX view,
  D3DXMATRIX projection,
  float minViewDistance,
  float maxViewDistance,
  float cameraFOV,
  D3DXVECTOR3 cameraVec
) {
  // Create a view matrix that transforms world space to view space.
  // Use a right-handed coordinate system.
  auto posVec = D3DXVECTOR3 { 0.0f, 0.0f, 0.0f };  // position where to look at
  auto axiVec = D3DXVECTOR3 { 0.0f, 1.0f, 0.0f };  // positive y-axis is up
  D3DXMatrixLookAtRH(&view, &cameraVec, &posVec, &axiVec);
  HRESULT result = pD3D9Device->SetTransform(D3DTS_VIEW, &view);
  if (FAILED(result)) {
    DXTRACE_ERR_MSGBOX(
      L"SetupTransform failed for view-space matrix",
      E_FAIL
    );
  }

  // Create a projection matrix that defines the view frustrum.
  // It transforms the view space to projection space.
  RECT rc { };
  GetClientRect(hWnd, &rc);
  D3DXMatrixPerspectiveFovRH(
    &projection,
    D3DXToRadian(cameraFOV),
    static_cast<FLOAT>(rc.right) / rc.bottom,
    minViewDistance,
    maxViewDistance
  );
  result = pD3D9Device->SetTransform(D3DTS_PROJECTION, &projection);
  if (FAILED(result)) {
    DXTRACE_ERR_MSGBOX(
      L"SetupTransform failed for view-projection matrix",
      E_FAIL
    );
  }

  D3DXMatrixIdentity(&view);

  // Update state
  GetAppCtx()->SetCurrentViewMatrix(view);
  GetAppCtx()->SetCurrentProjectionMatrix(projection);

  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: AppSetupD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT AppSetupD3D(AppCtx *appCtx, HWND hWnd, bool force)
{
  auto *pD3D9Device = appCtx->GetD3D9Device();

  // if we already have a device, try resetting it
  if (appCtx->GetD3D9Inited() && pD3D9Device) {
    if (FAILED(pD3D9Device->Reset(appCtx->GetEnumerator()->GetRefPP()))) {
      // TODO: print warning?
      pD3D9Device->Release();
    }
  } else if (appCtx->GetD3D9Inited() && !pD3D9Device) {
    appCtx->SetD3D9Inited(false);
    return DXTRACE_ERR_MSGBOX(
      L"AppInitD3D: Somehow initialized but no device pointer!",
      E_FAIL
    );
  }

  // Normal creation or deferred creation if failed to reset
  if (!pD3D9Device) {
    pD3D9Device = appCtx->GetEnumerator()->CreateRefDevice(hWnd, false);
    appCtx->SetDeviceCreateCalled(true);
    if (!pD3D9Device) {
      appCtx->SetD3D9Inited(false);
      return E_FAIL;
    }
    appCtx->SetD3D9Device(pD3D9Device);
    appCtx->SetDeviceCreated(true);
  }

  RECT    rc;
  HRESULT hr;

  //// Create a viewport which hold information about which region to draw to.
  GetClientRect(hWnd, &rc);
  D3DVIEWPORT9 viewport = {
    .X = 0, // start at top left corner
    .Y = 0,
    .Width  = (DWORD)rc.right,   // use the entire window
    .Height = (DWORD)rc.bottom,  // ..
    .MinZ   = 0.1f,
    .MaxZ   = appCtx->GetMaxViewDistance(), 
  };
  hr = pD3D9Device->SetViewport(&viewport);
  if (FAILED(hr)) {
    return DXTRACE_ERR_MSGBOX(
      L"AppInitD3D SetViewport: failed",
      hr
    );
  }

  // Setup word-view and projection transforms
  hr = AppSetupD3DTransform(
    hWnd,
    pD3D9Device,
    appCtx->GetCurrentViewMatrix(),
    appCtx->GetCurrentProjectionMatrix(),
    appCtx->GetMinViewDistance(),
    appCtx->GetMaxViewDistance(),
    appCtx->GetCameraFOV(),
    appCtx->GetCameraVec()
  );
  if (FAILED(hr)) {
    return DXTRACE_ERR_MSGBOX(
      L"AppInitD3D AppSetupD3DTransform: failed",
      hr
    );
  }

  pD3D9Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  pD3D9Device->SetRenderState(D3DRS_FOGENABLE, false);
  pD3D9Device->SetRenderState(D3DRS_AMBIENT, 0xFF323232);
  pD3D9Device->SetRenderState(D3DRS_COLORVERTEX, 1);

  appCtx->SetD3D9Inited(true);
  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: AppSetupRemix()
// Desc: Setup Remix - Post Initialization
//-----------------------------------------------------------------------------
HRESULT AppSetupRemix(
  AppCtx *appCtx,
  PFN_remixapi_BridgeCallback beginSceneCallback,
  PFN_remixapi_BridgeCallback endSceneCallback
 ) {
  //// Setup callbacks
  remixapi_ErrorCode r;
  r = remixapi::bridge_setRemixApiCallbacks(beginSceneCallback, endSceneCallback, nullptr);
  if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
    return DXTRACE_ERR_MSGBOX(
      L"AppSetupRemix bridge_setRemixApiCallbacks() failed: %d\n",
      r
    );
  }

  //// Get current window size
  RECT rc {};
  GetClientRect(appCtx->GetHWNDFocus(), &rc);

  //// Add some lights
  #pragma warning(push)
  #pragma warning(disable : 26812)

  AppRemixInterface *remix = appCtx->GetRemix();

  remix->app.sphereLight = {
    .sType            = REMIXAPI_STRUCT_TYPE_LIGHT_INFO_SPHERE_EXT,
    .pNext            = NULL,
    // .position      = {-80 , 7 , 0},
    .position         = {0 , 0 , 0},
    .radius           = 0.3f,
    .shaping_hasvalue = FALSE,
    .shaping_value    = { 0 },
  };
  remix->app.lightInfo = {
    .sType    = REMIXAPI_STRUCT_TYPE_LIGHT_INFO,
    .pNext    = &remix->app.sphereLight,
    .hash     = 0x3,
    .radiance = {0, 0, 0},
  };
  r = remix->CreateLight(&remix->app.lightInfo, &remix->app.sceneLight);
  if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
    return DXTRACE_ERR_MSGBOX(
      L"AppSetupRemix CreateLight() failed: %d\n",
      r
    );
  }

  //// Setup initial camera information 
  D3DXVECTOR3 cameraVec = appCtx->GetCameraVec();
  remix->app.parametersForCamera = {
    .sType = { REMIXAPI_STRUCT_TYPE_CAMERA_INFO_PARAMETERIZED_EXT },
    .pNext         = { nullptr },
    .position      = { cameraVec.x, cameraVec.y, cameraVec.z },
    .forward       = { 0, 0, 1 },
    .up            = { 0, 1, 0 },
    .right         = { 1, 0, 0 },
    .fovYInDegrees = appCtx->GetCameraFOV(),
    .aspect        = (FLOAT)(rc.right) / (FLOAT)(rc.bottom),
    .nearPlane     = appCtx->GetMinViewDistance(),
    .farPlane      = appCtx->GetMaxViewDistance(),
  };

  // Remix API command info
  remix->app.cameraInfo = {
    .sType = REMIXAPI_STRUCT_TYPE_CAMERA_INFO,
    .pNext = &remix->app.parametersForCamera,
    .type  = REMIXAPI_CAMERA_TYPE_WORLD
  };
  #pragma warning(pop)

  // Copy saved transforms to remix->app.cameraInfo
  memcpy(
    remix->app.cameraInfo.view, 
    appCtx->GetCurrentViewMatrix().m,
    sizeof(float[4][4])
  );
  memcpy(
    remix->app.cameraInfo.projection,
    appCtx->GetCurrentProjectionMatrix().m,
    sizeof(float[4][4])
  );

  r = remix->SetupCamera(&remix->app.cameraInfo);
  if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
    return DXTRACE_ERR_MSGBOX(
      L"AppSetupRemix SetupCamera() failed: %d\n",
      r
    );
  }

  // NOTE: if we don't pass the transforms to remix->app.cameraInfo view, it will use the ones
  // set by SetTransform
  // appCtx->GetD3D9Device()->SetTransform(D3DTS_VIEW, &view);
  // appCtx->GetD3D9Device()->SetTransform(D3DTS_PROJECTION, &projection);

  return r;
}




//-----------------------------------------------------------------------------
// Name: AppBeginSceneCallback()
// Desc: Called for every invocation of IDirect3DDevice9->BeginScene()
//-----------------------------------------------------------------------------
VOID __cdecl AppBeginSceneCallback() {
  static D3DXVECTOR3 posVec = { 0.0f, 0.0f, 0.0f };
  static D3DXVECTOR3 axiVec = { 0.0f, 1.0f, 0.0f };
  static auto remix     = GetAppCtx()->GetRemix();
  D3DXVECTOR3 camVec    = GetAppCtx()->GetCameraVec();
  D3DXMATRIX  view      = GetAppCtx()->GetCurrentViewMatrix();

//// Camera Animation Demo
#if 1
  // Update view transform for new cameraVec
  posVec.x += 0.1f;
  camVec.z += 0.1f;
  camVec.y += 0.2f;
  camVec.x -= 0.2f;
  D3DXMatrixLookAtRH(&view, &camVec, &posVec, &axiVec);

  // Copy updated transform matrix into remix cameraInfo and send
  memcpy(remix->app.cameraInfo.view, view.m, sizeof(view.m));
  remix->SetupCamera(&remix->app.cameraInfo);

  // Update context with new transform data
  GetAppCtx()->SetCurrentViewMatrix(view);
  GetAppCtx()->SetCameraVec(camVec);
#endif
}

//-----------------------------------------------------------------------------
// Name: AppEndSceneCallback()
// Desc: Called for every invocation of IDirect3DDevice9->EndScene()
//-----------------------------------------------------------------------------
VOID __cdecl AppEndSceneCallback() {
  static auto remix = GetAppCtx()->GetRemix();
  static const auto meshloader = GetAppCtx()->GetMeshLoader();


  // Remix API doesn't allow updates
  // So we stupidly recreate it for each frame because why not ...
  remix->DestroyLight(remix->app.sceneLight);
 
  //// Strobe ligting demo
  //if (!remix->app.sphereLightDirection) {
  //  if (remix->app.sphereLight.position.x > -50.0f) {
  //    remix->app.sphereLight.position.x -= 0.1f;
  //    remix->app.lightInfo.radiance.y += 0.05f;
  //    remix->app.lightInfo.radiance.x = 0;
  //  } else {
  //    remix->app.lightInfo.radiance.y = 0;
  //    remix->app.lightInfo.radiance.x = 0;
  //    remix->app.sphereLightDirection = 1;
  //  }
  //} else {
  //  if (remix->app.sphereLight.position.x < 50.0f) {
  //    remix->app.sphereLight.position.x += 0.1f;
  //    remix->app.lightInfo.radiance.x += 0.05f;
  //    remix->app.lightInfo.radiance.y = 0;
  //  } else {
  //    remix->app.lightInfo.radiance.y = 0;
  //    remix->app.lightInfo.radiance.x = 0;
  //    remix->app.sphereLightDirection = 0;
  //  }
  //}
  remix->app.sphereLight.position.x = 0.0f;
  remix->app.sphereLight.position.y = 0.0f;
  remix->app.sphereLight.position.z = 0.0f;
  remix->app.lightInfo.radiance.x = 0.0f;
  remix->app.lightInfo.radiance.y = 300.0f;
  remix->app.lightInfo.radiance.z = 200.0f;

  // Draw OBJ mesh instances
  if (meshloader) [[likely]]
    meshloader->GetMesh()->DrawSubset(0);

  // Let there be light
  remix->CreateLight(&remix->app.lightInfo, &remix->app.sceneLight);
  remix->DrawLightInstance(remix->app.sceneLight);
}

//-----------------------------------------------------------------------------
// Name: AppRender()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID AppRender(IDirect3DDevice9 *pD3D9Device) {
  pD3D9Device->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
  pD3D9Device->BeginScene();
  // Hooks add calls between Begin and End
  pD3D9Device->EndScene();
  pD3D9Device->Present(NULL, NULL, NULL, NULL);
}

//-----------------------------------------------------------------------------
// Name: AppInit()
// Desc: Initializes firt-time context and sets init state, plus extras
//-----------------------------------------------------------------------------
AppCtx *AppInit(const wchar_t *appname, bool console) {
  AppCtx *appCtx = GetAppCtx();
  appCtx->SetInited(true);
  
  if (console && AllocConsole()) {
    FILE* f = nullptr;
    (void) freopen_s(&f, "CONIN$",  "r", stdin);
    (void) freopen_s(&f, "CONOUT$", "w", stdout);
    (void) freopen_s(&f, "CONOUT$", "w", stderr);
    SetConsoleTitleW(appname);
  }

  return appCtx;
}


//-----------------------------------------------------------------------------
// Name: AppMain()
// Desc: Entrypoint
//-----------------------------------------------------------------------------
INT AppMain(
  _In_     HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_     LPWSTR lpCmdLine,
  _In_     int nShowCmd
) {

  HRESULT hr = 0;
  AppCtx *appCtx = AppInit(AppName, false);

  //// Create Window
  HWND appWindow = NULL;
  {
    hr = AppCreateWindowW(AppName, NULL, NULL, NULL, 100, 100);
    if (FAILED(hr)) {
      return DXTRACE_ERR_MSGBOX(
        L"Failed to create application window",
        E_FAIL
      );
    }
    appWindow = appCtx->GetHWNDFocus();
    ShowWindow(appWindow, SW_SHOWDEFAULT);
    UpdateWindow(appWindow);
  }

  //// Direct3D / Remix / Managers
  IDirect3DDevice9 *pD3D9Device = nullptr;
  {
    hr = AppSetupD3D(appCtx, appWindow);
    if (FAILED(hr)) {
      return DXTRACE_ERR_MSGBOX(
        L"AppSetupD3D: failed",
        hr
      );
    }
    pD3D9Device = appCtx->GetD3D9Device();

    hr = AppSetupRemix(appCtx, AppBeginSceneCallback, AppEndSceneCallback);
    if (FAILED(hr)) {
      return DXTRACE_ERR_MSGBOX(
        L"AppSetupRemix: failed",
        hr
      );
    }

    auto meshloader = appCtx->GetMeshLoader();
    std::filesystem::path objfilepath("H:/Share/Sources/magos/game/models/teapot.obj"); // TODO: make configurable
    if (FAILED(meshloader->Create(pD3D9Device, objfilepath))) {
      return DXTRACE_ERR_MSGBOX(
        L"WARN: Failed to parse OBJ",
        E_FAIL
      );
    }
  }
  
  // Export device to any participating DLL's
  // (no-op) if no participating DLL's
  pD3D9Device = ExportDevice(pD3D9Device);

  //// Enter the message loop
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while(msg.message != WM_QUIT) {
    if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    if (!appCtx->GetIsRenderingPaused()) {
      AppRender(pD3D9Device);
    }
  }
  UnregisterClassW(L"Direct3DWindowClass", appCtx->GetHInstance()); 
  return S_OK;
}

