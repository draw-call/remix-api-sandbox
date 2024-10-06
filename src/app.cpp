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

#include "app.h"

template <typename T>
unsigned int GetByteSize(const std::vector<T>& vec) {
  return sizeof(vec[0]) * vec.size();
}

//-----------------------------------------------------------------------------
// Name: CreateBuffer()
// Desc: Create vertex buffer from CUSTOMVERTEX (for now)
//-----------------------------------------------------------------------------
IDirect3DVertexBuffer9* CreateBuffer(const std::vector<CUSTOMVERTEX>& vertices) {
  IDirect3DVertexBuffer9* buffer { };
  void* data { };
  HRESULT result;
  result = g_pd3dDevice->CreateVertexBuffer(
    GetByteSize(vertices), // vector size in bytes
    0,                     // data usage
    VertexStructFVF,       // FVF of the struct
    D3DPOOL_DEFAULT,       // use default pool for the buffer
    &buffer,               // receiving buffer
    nullptr                // special shared handle
  );
  if (FAILED(result)) {
    return nullptr;
  }

  // Create a data pointer for copying the vertex data
  result = buffer->Lock(
    0,                     // byte offset
    GetByteSize(vertices), // size to lock
    &data,                 // receiving data pointer
    0                      // special lock flags
  );
  if (FAILED(result)) {
    return nullptr;
  }
  memcpy(data, vertices.data(), GetByteSize(vertices));
  buffer->Unlock();

  return buffer;
}

//-----------------------------------------------------------------------------
// Name: SetupTransform()
// Desc: Setup initial view transforms
//-----------------------------------------------------------------------------
bool SetupTransform()
{
  // Create a view matrix that transforms world space to view space.
  // Use a right-handed coordinate system.
  auto cam_vec = D3DXVECTOR3 { 0.0f, 0.0f, -20.0f }; // "camera" position
  auto pos_vec = D3DXVECTOR3 { 0.0f, 0.0f, 0.0f };   // position where to look at
  auto axi_vec = D3DXVECTOR3 { 0.0f, 1.0f, 0.0f };   // positive y-axis is up
  D3DXMatrixLookAtRH(
    &g_view,
    &cam_vec,
    &pos_vec,
    &axi_vec
  );
  HRESULT result = g_pd3dDevice->SetTransform(D3DTS_VIEW, &g_view);
  if (FAILED(result)) {
    return false;
  }

  // Create a projection matrix that defines the view frustrum.
  // It transforms the view space to projection space.
  #if 1
  RECT clientRect{ };
  GetClientRect(g_hWnd, &clientRect);
  // Create a perspective projection using a right-handed coordinate system.
  D3DXMatrixPerspectiveFovRH(
    &g_projection,       // receiving matrix
    D3DXToRadian(60.0f), // field of view in radians
    static_cast<FLOAT>(clientRect.right) / clientRect.bottom,  // aspect ratio (width / height)
   -1000.0f,  // minimum view distance
    1000.0f   // maximum view distance
  );
  result = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_projection);
  if (FAILED(result)) {
    return false;
  }

  // World-View Identity
  D3DXMatrixIdentity(&g_view);
  #endif

  // result = g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);

  return true;
}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd) {
  if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) {
    return E_FAIL;
  }

  RECT clientRect{ };
  GetClientRect(hWnd, &clientRect);

  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

  // Create the D3DDevice
  if(FAILED(
    g_pD3D->CreateDevice(
      D3DADAPTER_DEFAULT,
      D3DDEVTYPE_HAL,
      hWnd,
      D3DCREATE_HARDWARE_VERTEXPROCESSING,
      &d3dpp,
      &g_pd3dDevice
    ))) {
      return E_FAIL;
  }

  //// Create a viewport which hold information about which region to draw to.
  #if 1
  D3DVIEWPORT9 viewport = {
    .X = 0, // start at top left corner
    .Y = 0,
    .Width  = (DWORD)clientRect.right,   // use the entire window
    .Height = (DWORD)clientRect.bottom,  // ..
    .MinZ   = -1000.0f,           // minimum view distance
    .MaxZ   = 1000.0f,            // maximum view distance
  };
  HRESULT result = g_pd3dDevice->SetViewport(&viewport);
  if (FAILED(result)) {
    return E_FAIL;
  }
  #endif

  g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
  g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xFF323232);
  g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, 1);

  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup() {
  if (g_vertexBuffer != NULL) {
    g_vertexBuffer->Release();
  }
  if (g_pd3dDevice != NULL) {
    g_pd3dDevice->Release();
  }
  if (g_pD3D != NULL) {
    g_pD3D->Release();
  }
  if (g_meshloader != NULL) {
    g_meshloader.reset();
  }
}

VOID beginSceneCallback() {
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
  // g_remix.SetupCamera(&cameraInfo);
}


//-----------------------------------------------------------------------------
// Name: endSceneCallback()
// Desc: primary remix api callback
//-----------------------------------------------------------------------------
VOID endSceneCallback() {
  remixapi_InstanceInfo g_remix_meshInstanceInfo = {
    .sType = REMIXAPI_STRUCT_TYPE_INSTANCE_INFO,
    .categoryFlags = 0,
    .mesh = g_remix_scene_mesh,
    .transform = {{
      {1,0,0,0},
      {0,1,0,0},
      {0,0,1,0}
    }},
    .doubleSided = 1,
  };
  
  // Remix API doesn't allow updates
  // So we stupidly recreate it for each frame because why not ...
  g_remix.DestroyLight(g_remix_scene_light);
 
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


  // Draw Remix Mesh Instance(s)
  g_remix.DrawInstance(&g_remix_meshInstanceInfo);

  // Draw OBJ mesh instances
  (g_meshloader)->GetMesh()->DrawSubset(0);

  // Let there be light
  g_remix.CreateLight(&g_remix_lightInfo, &g_remix_scene_light);
  g_remix.DrawLightInstance(g_remix_scene_light);
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render() {
  g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
  g_pd3dDevice->BeginScene();
  g_pd3dDevice->SetFVF(VertexStructFVF);
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 3);
  g_pd3dDevice->EndScene();
  g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch(msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_QUIT:
      Cleanup();
      return 0;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: Entrypoint
//-----------------------------------------------------------------------------
INT wWinMain(
  _In_     HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_     LPWSTR lpCmdLine,
  _In_     int nShowCmd
) {
  UNREFERENCED_PARAMETER(hInstance);
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  UNREFERENCED_PARAMETER(nShowCmd);

  // Register the window class
  WNDCLASSEX wc = {
    sizeof(WNDCLASSEX),
    CS_VREDRAW | CS_HREDRAW,
    MsgProc,
    0L,
    0L,
    GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
    "Remix Test",
    NULL
  };
  RegisterClassEx(&wc);

  // Create the application's window
  g_hWnd = CreateWindowW(
    L"Remix Test",
    L"Remix Test",
    WS_OVERLAPPEDWINDOW,
    100,
    100,
    1024,
    768,
    NULL,
    NULL,
    wc.hInstance,
    NULL
  );

  ShowWindow(g_hWnd, SW_SHOWDEFAULT);
  UpdateWindow(g_hWnd);

  // Init API
  remixapi_ErrorCode r;
  r = remixapi::bridge_initRemixApi(&g_remix);
  if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
    printf("remixapi::bridge_initRemixApi: failed: %d\n", r);
    return r;
  }
  r = remixapi::bridge_setRemixApiCallbacks(beginSceneCallback, endSceneCallback, nullptr);
  if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
    printf("remixapi::bridge_setRemixApiCallbacks: failed: %d\n", r);
    return r;
  }

  // Initialize Direct3D
  if(SUCCEEDED(InitD3D(g_hWnd))) {
    // Create non-remix vertex buffer
    std::vector<CUSTOMVERTEX> vertices { 
      {
        // Bottom left
        CUSTOMVERTEX{-10.0f, -10.0f, 10.0f, 0, 0, -1, 0, 0 },
        // Top left
        CUSTOMVERTEX{-10.0f,  10.0f, 10.0f, 0, 0, -1, 0, 0 },
        // Top right
        CUSTOMVERTEX{ 10.0f,  10.0f, 10.0f, 0, 0, -1, 0, 0 },
      }
    };
    if (!(g_vertexBuffer = CreateBuffer(vertices))) {
      return -1;
    }
    HRESULT result = g_pd3dDevice->SetStreamSource(
      0,
      g_vertexBuffer.Get(), // pass the vertex buffer
      0,                    // no offset
      sizeof(CUSTOMVERTEX)
    );

    // Setup word-view and projection transforms
    SetupTransform();

    // Setup OBJ meshloader
    g_meshloader.reset(new CMeshLoader());
    std::filesystem::path objfilepath("H:/Share/Sources/magos/game/models/teapot.obj");
    if (FAILED(g_meshloader->Create(g_pd3dDevice, objfilepath))) {
      return -1;
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

    // Create remix hardcoded verts (another instance)
    constexpr remixapi_HardcodedVertex verts[] = {
      makeVertex( 5,  5, 0.1f),
      makeVertex(-5,  5, 0.1f),
      makeVertex(-5, -5, 0.1f),
    };
    g_remix_triangles = {
      .vertices_values   = verts,
      .vertices_count    = ARRAYSIZE(verts),
      .indices_values    = NULL,
      .indices_count     = 0,
      .skinning_hasvalue = FALSE,
      .skinning_value    = { 0 },
      .material          = NULL,
    };
    g_remix_meshInfo = {
      .sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
      .pNext = NULL,
      .hash  = 0x1,
      .surfaces_values = &g_remix_triangles,
      .surfaces_count  = 1,
    };

    r = g_remix.CreateMesh(&g_remix_meshInfo, &g_remix_scene_mesh);
    if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
      printf("remix::CreateMesh() failed: %d\n", r);
      return r;
    }
    r = g_remix.CreateLight(&g_remix_lightInfo, &g_remix_scene_light);
    if (r != REMIXAPI_ERROR_CODE_SUCCESS) {
      printf("remix::CreateLight() failed: %d\n", r);
      return r;
    }

    // Enter the message loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while(msg.message != WM_QUIT) {
      if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      else Render();
    }
  }

  UnregisterClassW(L"Remix Test", wc.hInstance);
  return 0;
}
