//--------------------------------------------------------------------------------------
// File: MeshLoader.h
//
// Wrapper class for ID3DXMesh interface. Handles loading mesh data from an .obj file
// and resource management for material textures.
//
// Copyright (c) Microsoft Corporation.
// Copyright (c) Dayton Pidhirney.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------
#pragma once

#include <filesystem>
#include <d3dx9.h>

#include "cgarray.hpp"

class VERTEX;
class CacheEntry;
class Material;
class CMeshLoader;

// Vertex format
struct VERTEX {
  D3DXVECTOR3 position;
  D3DXVECTOR3 normal;
  D3DXVECTOR2 texcoord;
};

// Used for a hashtable vertex cache when creating the mesh from a .obj file
struct CacheEntry {
  UINT index;
  CacheEntry* pNext;
};

// Material properties per mesh subset
struct Material {
  WCHAR strName[MAX_PATH];

  D3DXVECTOR3 vAmbient;
  D3DXVECTOR3 vDiffuse;
  D3DXVECTOR3 vSpecular;

  int nShininess;
  float fAlpha;

  bool bSpecular;

  WCHAR strTexture[MAX_PATH];
  IDirect3DTexture9* pTexture;
  D3DXHANDLE hTechnique;
};

class CMeshLoader {
public:
  CMeshLoader();
  ~CMeshLoader();

  HRESULT Create(IDirect3DDevice9* pd3dDevice, std::filesystem::path &objfilename);
  void Destroy();

  UINT GetNumMaterials() const
  {
    return m_Materials.GetSize();
  }
  Material* GetMaterial(UINT iMaterial)
  {
    return m_Materials.GetAt(iMaterial);
  }

  ID3DXMesh* GetMesh()
  {
    return m_pMesh;
  }
  WCHAR* GetMediaDirectory()
  {
    return m_strMediaDir;
  }

private:
  HRESULT LoadGeometryFromOBJ(std::filesystem::path  &objfilepath);
  HRESULT LoadMaterialsFromMTL(std::filesystem::path &mtlfilepath);

  DWORD AddVertex(UINT hash, VERTEX* pVertex);
  void  DeleteCache();
  void  InitMaterial(Material* pMaterial);

  IDirect3DDevice9* m_pd3dDevice; // Direct3D Device object associated with this mesh
  ID3DXMesh* m_pMesh;             // Encapsulated D3DX Mesh

  CGrowableArray<CacheEntry*> m_VertexCache; // Hashtable cache for locating duplicate vertices
  CGrowableArray<VERTEX> m_Vertices;         // Filled and copied to the vertex buffer
  CGrowableArray<DWORD> m_Indices;           // Filled and copied to the index buffer
  CGrowableArray<DWORD> m_Attributes;        // Filled and copied to the attribute buffer
  CGrowableArray<Material*> m_Materials;     // Holds material properties per subset

  WCHAR m_strMediaDir[MAX_PATH]; // Directory where the mesh was found
};
