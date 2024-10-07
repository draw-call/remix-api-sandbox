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