// D3D9 Remix Demo / Playground / Sandbox / Dogshit
// watbulb
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <Windows.h>

#include <vector>
#include <memory>
#include <filesystem>

#include <d3dx9.h>
#include <d3d9types.h>

#include "app_remix.hpp"
#include "dxerror.hpp"
#include "meshloader.hpp"
#include "state.hpp"
#include "window.hpp"

#ifndef AppMain
#define AppMain wWinMain
#endif


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
  RECT clientRect { };
  GetClientRect(hWnd, &clientRect);
  D3DXMatrixPerspectiveFovRH(
    &projection,
    D3DXToRadian(60.0f), // FOV
    // aspect ratio (width / height)
    static_cast<FLOAT>(clientRect.right) / clientRect.bottom,
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

  // World-View Identity
  D3DXMatrixIdentity(&view);

  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: AppInitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT AppInitD3D(HWND hWnd, bool force = false)
{
  AppCtx *appCtx    = GetAppCtx();
  auto *pD3D9Device = appCtx->GetD3D9Device();

  // if we already have a device, try resetting it
  if (appCtx->GetD3D9Inited() && pD3D9Device) {
    DXTRACE_MSG(L"WARN: Calling AppInitD3D Twice!");
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
    .MinZ   = appCtx->GetMinViewDistance(),
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
// Name: AppRemixSetup()
// Desc: Setup Remix - Post Initialization
//-----------------------------------------------------------------------------
HRESULT AppRemixSetup(
  PFN_remixapi_BridgeCallback beginSceneCallback,
  PFN_remixapi_BridgeCallback endSceneCallback
 ) {
  // Init Remix callbacks API
  remixapi_ErrorCode r;
  r = remixapi::bridge_setRemixApiCallbacks(beginSceneCallback, endSceneCallback, nullptr);
  if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
    return r;
  }
  // Add some lights
  #pragma warning(push)
  #pragma warning(disable : 26812)
  g_remix_sphereLight = {
    .sType    = REMIXAPI_STRUCT_TYPE_LIGHT_INFO_SPHERE_EXT,
    .pNext    = NULL,
    .position = {-80 , 7 , 0},
    .radius   = 0.3f,
    .shaping_hasvalue = FALSE,
    .shaping_value    = { 0 },
  };
  #pragma warning(pop)
  g_remix_lightInfo = {
    .sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO,
    .pNext = &g_remix_sphereLight,
    .hash  = 0x3,
    .radiance = {0, 0, 0},
  };
  r = GetAppCtx()->GetRemix().CreateLight(&g_remix_lightInfo, &g_remix_scene_light);
  if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
    printf("remix::CreateLight() failed: %d\n", r);
    return r;
  }

  return r;
}

VOID AppBeginSceneCallback() {
  static remixapi_CameraInfoParameterizedEXT parametersForCamera = {
    .sType = REMIXAPI_STRUCT_TYPE_CAMERA_INFO_PARAMETERIZED_EXT,
    .position = { 0, 0, -30 },
    .forward = { 0,0,1 },
    .up    = { 0,1,0 },
    .right = { 1,0,0 },
    .fovYInDegrees = 60,
    .aspect = (float)1024 / (float)768,
    .nearPlane = -1000.0f,
    .farPlane  = 1000.0f,
  };
  remixapi_CameraInfo cameraInfo = {
    .sType = REMIXAPI_STRUCT_TYPE_CAMERA_INFO,
    .pNext = &parametersForCamera,
    .type = REMIXAPI_CAMERA_TYPE_WORLD 
  };
  // GetAppCtx()->GetRemix().SetupCamera(&cameraInfo);

}


//-----------------------------------------------------------------------------
// Name: endSceneCallback()
// Desc: primary remix api callback
//-----------------------------------------------------------------------------
VOID AppEndSceneCallback() {
  const auto remix      = GetAppCtx()->GetRemix();
  const auto meshloader = GetAppCtx()->GetMeshLoader();

  // Remix API doesn't allow updates
  // So we stupidly recreate it for each frame because why not ...
  remix.DestroyLight(g_remix_scene_light);
 
  if (!g_remix_sphereLightDirection) {
    if (g_remix_sphereLight.position.x > -50.0f) {
      g_remix_sphereLight.position.x -= 0.1f;
      g_remix_lightInfo.radiance.y += 0.3f;
      g_remix_lightInfo.radiance.x = 0;
    } else {
      g_remix_lightInfo.radiance.y = 0;
      g_remix_lightInfo.radiance.x = 0;
      g_remix_sphereLightDirection = 1;
    }
  } else {
    if (g_remix_sphereLight.position.x < 50.0f) {
      g_remix_sphereLight.position.x += 0.1f;
      g_remix_lightInfo.radiance.x += 0.3f;
      g_remix_lightInfo.radiance.y = 0;
    } else {
      g_remix_lightInfo.radiance.y = 0;
      g_remix_lightInfo.radiance.x = 0;
      g_remix_sphereLightDirection = 0;
    }
  }

  // Draw OBJ mesh instances
  if (meshloader) {
    meshloader->GetMesh()->DrawSubset(0);
  }

  // Let there be light
  remix.CreateLight(&g_remix_lightInfo, &g_remix_scene_light);
  remix.DrawLightInstance(g_remix_scene_light);
}

//-----------------------------------------------------------------------------
// Name: AppRender()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID AppRender(IDirect3DDevice9 *pD3D9Device) {
  pD3D9Device->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
  pD3D9Device->BeginScene();
  // XXX: Hooks add calls between Begin and End
  pD3D9Device->EndScene();
  pD3D9Device->Present(NULL, NULL, NULL, NULL);
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
  UNREFERENCED_PARAMETER(hInstance);
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  UNREFERENCED_PARAMETER(nShowCmd);

  FILE* f = nullptr;
  (void) freopen_s(&f, "CONIN$", "r",  stdin);
  (void) freopen_s(&f, "CONOUT$", "w", stdout);
  (void) freopen_s(&f, "CONOUT$", "w", stderr);

  // Grab appCtx early
  AppCtx *appCtx = GetAppCtx();
  auto remix = appCtx->GetRemix();

  // TODO: AppInit()
  appCtx->SetInited(true);

  HRESULT hr = 0;

  //// Create Window
  HWND appWindow = NULL;
  {
    hr = AppCreateWindow(L"remix-api-sandbox", NULL, NULL, NULL, 100, 100);
    if (FAILED(hr)) {
      return DXTRACE_ERR_MSGBOX(
        L"Failed to create application window",
        E_FAIL
      );
    } else {
      appWindow = appCtx->GetHWNDFocus();
    }
    ShowWindow(appWindow, SW_SHOWDEFAULT);
    UpdateWindow(appWindow);
  }

  //// Direct3D / Remix / Managers
  {
    // Initialize Direct3D
    hr = AppInitD3D(appWindow);
    if (FAILED(hr)) {
      return DXTRACE_ERR_MSGBOX(L"AppInitD3D: failed", hr);
    }

    // Setup Remix
    hr = AppRemixSetup(AppBeginSceneCallback, AppEndSceneCallback);
    if (FAILED(hr)) {
      return DXTRACE_ERR_MSGBOX(L"AppRemixSetup: failed", hr);
    }

    // Setup OBJ meshloader
    std::filesystem::path objfilepath("H:/Share/Sources/magos/game/models/teapot.obj");
    if (FAILED(appCtx->GetMeshLoader()->Create(appCtx->GetD3D9Device(), objfilepath))) {
      return DXTRACE_ERR_MSGBOX(L"WARN: Failed to parse OBJ", E_FAIL);
    }
  }

  //// Enter the message loop
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while(msg.message != WM_QUIT) {
    if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    if (!appCtx->GetIsRenderingPaused()) {
      AppRender(appCtx->GetD3D9Device());
    }
  }
  UnregisterClassW(L"Direct3DWindowClass", appCtx->GetHInstance()); 
  return S_OK;
}

