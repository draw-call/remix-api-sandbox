#pragma once
#include <wrl.h>
#include <d3dx9.h>

#include "remixapi/bridge_remix_api.h"

#include "dxvkcom.h"
#include "meshloader.hpp"

using namespace Microsoft::WRL;

// A structure for our custom vertex type
// (for non-remix vertex stuff)
struct CUSTOMVERTEX {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | /* D3DFVF_DIFFUSE | */ D3DFVF_TEX1)

// Window
static HWND g_hWnd = NULL;

// D3D
static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DXMATRIX g_view {};
static D3DXMATRIX g_projection {};
static ComPtr<IDirect3DVertexBuffer9> g_vertexBuffer {};
static const unsigned long VertexStructFVF = D3DFVF_CUSTOMVERTEX;

// Managers
static std::shared_ptr<CMeshLoader> g_meshloader;

// Remix
static HMODULE g_remix_dll = NULL;
static remixapi_Interface g_remix = { 0 };

static remixapi_LightHandle g_remix_scene_light = NULL;
static remixapi_MeshHandle g_remix_scene_mesh = NULL;

static remixapi_MeshInfo g_remix_meshInfo;
static remixapi_InstanceInfo g_remix_meshInstanceInfo;
static remixapi_MeshInfoSurfaceTriangles g_remix_triangles;
static remixapi_LightInfoSphereEXT g_remix_sphereLight;
static remixapi_LightInfo g_remix_lightInfo;

static bool g_remix_sphereLightDirection = 1;

constexpr remixapi_HardcodedVertex makeVertex(float x, float y, float z)
{
  remixapi_HardcodedVertex v = {
    { x, y, z },
    { 0, 0, -1 },
    { 1, 1 },
    0xFFFFFFFF
  };
  return v;
}
