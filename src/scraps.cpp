#if 0

template <typename T>
unsigned int GetByteSize(const std::vector<T>& vec) {
  return sizeof(vec[0]) * vec.size();
}

//-----------------------------------------------------------------------------
// Name: AppCreateVBuffer()
// Desc: Create vertex buffer from CUSTOMVERTEX (for now)
//-----------------------------------------------------------------------------
IDirect3DVertexBuffer9* AppCreateVBuffer(const std::vector<CUSTOMVERTEX>& vertices) {
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
  return E_FAIL;
}
HRESULT result = g_pd3dDevice->SetStreamSource(
  0,
  g_vertexBuffer.Get(), // pass the vertex buffer
  0,                    // no offset
  sizeof(CUSTOMVERTEX)
);
#endif

#if 0
    D3DXVECTOR3 newCameraPosition = D3DXVECTOR3(cdc::GameState->GetCamera()->effectPosition.___u0.vec128.m128_f32);
    D3DXVECTOR3 cameraRotation    = D3DXVECTOR3(cdc::GameState->GetCamera()->Rotation.___u0.vec128.m128_f32); // Assuming this is in radians

    if (frameNumber) {
      if (newCameraPosition[0] != 0 && newCameraPosition[1] != 513.163696f) {
        initialCameraPosition = newCameraPosition;
        frameNumber = false;
      }
      return m_pIDirect3DDevice9->EndScene();
    }
 
    D3DXVECTOR3 displacement   = ((newCameraPosition - initialCameraPosition));
    const auto  displacement_x = displacement.x;
    const auto  displacement_y = displacement.y;
    displacement.x = displacement.y;
    displacement.y = displacement_x;
    displacement.z = 0;
    
    D3DLIGHT9 light;
    {
      ZeroMemory(&light, sizeof(light));
      light.Type         = D3DLIGHT_SPOT;
      light.Diffuse      = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
      light.Position     = displacement * 0.4;
      light.Direction    = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
      light.Range        = 1000.0f;
      light.Attenuation0 = 0.0f;    // No constant inverse attenuation
      light.Attenuation1 = 0.125f;  // Only .125 inverse attenuation
      light.Attenuation2 = 0.0f;    // No square inverse attenuation
      light.Phi          = D3DXToRadian(40.0f);
      light.Theta        = D3DXToRadian(20.0f);
      light.Falloff      = 1.0f;

      m_pIDirect3DDevice9->SetLight(0, &light);
      m_pIDirect3DDevice9->LightEnable(0, TRUE);
    }
#endif