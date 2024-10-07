// D3D9 Remix Demo / Playground / Sandbox / Dogshit
// watbulb
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <Windows.h>

#include <vector>
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

#define D3DFVF_CUSTOMVERTEX \
  (D3DFVF_XYZ | D3DFVF_NORMAL | /* D3DFVF_DIFFUSE | */ D3DFVF_TEX1)

// Managers
// TODO: Move to AppState
static std::shared_ptr<CMeshLoader> g_meshloader;

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
  auto posVec = D3DXVECTOR3 { 0.0f, 0.0f, 0.0f };   // position where to look at
  auto axiVec = D3DXVECTOR3 { 0.0f, 1.0f, 0.0f };   // positive y-axis is up
  D3DXMatrixLookAtRH(&view, &cameraVec, &posVec, &axiVec);
  HRESULT result = pD3D9Device->SetTransform(D3DTS_VIEW, &view);
  if (FAILED(result)) {
    DXTRACE_ERR_MSGBOX(
      L"SetupTransform failed for view-space matrix", E_FAIL
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
      L"SetupTransform failed for view-projection matrix", E_FAIL
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
HRESULT AppInitD3D(HWND hWnd) {
  auto pD3D9Device = GetAppState()->GetEnumerator()->CreateRefDevice(hWnd, false);
  if (!pD3D9Device) {
    DXTRACE_ERR_MSGBOX(L"Failed to create D3D9Device!", E_FAIL);
  }
  GetAppState()->SetD3D9Device(pD3D9Device);
  GetAppState()->SetDeviceCreateCalled(true);
  GetAppState()->SetDeviceCreated(true);

  //// Create a viewport which hold information about which region to draw to.
  RECT rc;
  GetClientRect(hWnd, &rc);
  D3DVIEWPORT9 viewport = {
    .X = 0, // start at top left corner
    .Y = 0,
    .Width  = (DWORD)rc.right,    // use the entire window
    .Height = (DWORD)rc.bottom,   // ..
    .MinZ   = -1000.0f,           // minimum view distance
    .MaxZ   =  1000.0f,           // maximum view distance
  };
  HRESULT result = pD3D9Device->SetViewport(&viewport);
  if (FAILED(result)) {
    return E_FAIL;
  }

  pD3D9Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  pD3D9Device->SetRenderState(D3DRS_FOGENABLE, false);
  pD3D9Device->SetRenderState(D3DRS_AMBIENT, 0xFF323232);
  pD3D9Device->SetRenderState(D3DRS_COLORVERTEX, 1);

  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: AppCleanup()
// Desc: Releases all previously initialized objects
// TODO: Needs to be more robust ... probably
//-----------------------------------------------------------------------------
VOID AppCleanup() {
  if (g_meshloader != NULL) {
    g_meshloader.reset();
  }
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
  r = GetAppState()->GetRemix().CreateLight(&g_remix_lightInfo, &g_remix_scene_light);
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
  // GetAppState()->GetRemix().SetupCamera(&cameraInfo);

}


//-----------------------------------------------------------------------------
// Name: endSceneCallback()
// Desc: primary remix api callback
//-----------------------------------------------------------------------------
VOID AppEndSceneCallback() {
  const auto remix = GetAppState()->GetRemix();

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
  g_meshloader->GetMesh()->DrawSubset(0);

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

  HRESULT hr = 0;

  FILE* f = nullptr;
  (void) freopen_s(&f, "CONIN$", "r",  stdin);
  (void) freopen_s(&f, "CONOUT$", "w", stdout);
  (void) freopen_s(&f, "CONOUT$", "w", stderr);

  // Grab appState early
  AppState *appState = GetAppState();
  auto remix = appState->GetRemix();

  // TODO: AppInit()
  appState->SetInited(true);

  // Create Window
  HWND appWindow = NULL;
  {
    hr = AppCreateWindow(L"remix-api-sandbox", NULL, NULL, NULL, 100, 100);
    if (FAILED(hr)) {
      DXTRACE_ERR_MSGBOX(
        L"Failed to create application window",
        E_FAIL
      );
    } else {
      appWindow = appState->GetHWNDFocus();
    }

    ShowWindow(appWindow, SW_SHOWDEFAULT);
    UpdateWindow(appWindow);
  }

  //
  // Direct3D / Remix
  //
  {
    // Initialize Direct3D
    hr = AppInitD3D(appWindow);
    if (FAILED(hr)) {
      DXTRACE_ERR_MSGBOX(L"AppInitD3D: failed", hr);
    }

    // Setup word-view and projection transforms
    hr = AppSetupD3DTransform(
      appWindow,
      appState->GetD3D9Device(),
      appState->GetCurrentViewMatrix(),
      appState->GetCurrentProjectionMatrix(),
      appState->GetMinViewDistance(),
      appState->GetMaxViewDistance(),
      appState->GetCameraVec()
    );
    if (FAILED(hr)) {
      DXTRACE_ERR_MSGBOX(L"AppSetupD3DTransform: failed", hr);
    }

    // Initialize Remix
    hr = AppRemixSetup(AppBeginSceneCallback, AppEndSceneCallback);
    if (FAILED(hr)) {
      DXTRACE_ERR_MSGBOX(
        L"AppRemixSetup: failed",
        hr
      );
    }
  }

  // Setup OBJ meshloader
  g_meshloader.reset(new CMeshLoader());
  std::filesystem::path objfilepath("H:/Share/Sources/magos/game/models/teapot.obj");
  if (FAILED(g_meshloader->Create(appState->GetD3D9Device(), objfilepath))) {
    DXUTOutputDebugString(
      L"WARN: Failed to parse OBJ: %s\n",
      objfilepath.c_str()
    );
  }

  // Enter the message loop
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while(msg.message != WM_QUIT) {
    if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else AppRender(appState->GetD3D9Device());
  }
  AppCleanup();

  UnregisterClassW(L"Direct3DWindowClass", appState->GetHInstance());
  return 0;
}
