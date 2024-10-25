#pragma once

#include "app_remix.hpp"
#include "dxerror.hpp"
#include "meshloader.hpp"
#include "state.hpp"
#include "window.hpp"


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
);


//-----------------------------------------------------------------------------
// Name: AppSetupD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT AppSetupD3D(AppCtx *appCtx, HWND hWnd, bool force = false);


//-----------------------------------------------------------------------------
// Name: AppSetupRemix()
// Desc: Setup Remix - Post Initialization
//-----------------------------------------------------------------------------
HRESULT AppSetupRemix(
  AppCtx *appCtx,
  PFN_remixapi_BridgeCallback beginSceneCallback = nullptr,
  PFN_remixapi_BridgeCallback endSceneCallback   = nullptr
 );


//-----------------------------------------------------------------------------
// Name: AppBeginSceneCallback()
// Desc: Called for every invocation of IDirect3DDevice9->BeginScene()
//-----------------------------------------------------------------------------
VOID __cdecl AppBeginSceneCallback();


//-----------------------------------------------------------------------------
// Name: AppEndSceneCallback()
// Desc: Called for every invocation of IDirect3DDevice9->EndScene()
//-----------------------------------------------------------------------------
VOID __cdecl AppEndSceneCallback();


//-----------------------------------------------------------------------------
// Name: AppRender()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID AppRender(IDirect3DDevice9 *pD3D9Device);


//-----------------------------------------------------------------------------
// Name: AppInit()
// Desc: Initializes firt-time context and sets init state, plus extras
//-----------------------------------------------------------------------------
AppCtx *AppInit(const wchar_t *appname, bool console = true);


//-----------------------------------------------------------------------------
// Name: AppMain()
// Desc: Entrypoint
//-----------------------------------------------------------------------------
INT AppMain(
  _In_     HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_     LPWSTR lpCmdLine,
  _In_     int nShowCmd
);
