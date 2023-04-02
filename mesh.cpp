void Mesh::Initialize(Directx *directx) {
  // Vertex vertices[4] = {
  //   Vertex{XMFLOAT2{-1, -1}, XMFLOAT2{0, 1}},
  //   Vertex{XMFLOAT2{-1, 1}, XMFLOAT2{0, 0}},
  //   Vertex{XMFLOAT2{1, 1}, XMFLOAT2{1, 0}},
  //   Vertex{XMFLOAT2{1, -1}, XMFLOAT2{1, 1}}
  // };
  // int indices[6] = {0, 1, 2, 0, 2, 3};

  // m_index_buffer = directx->CreateBuffer(D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, 0, &indices[0], sizeof(indices));

  // // We sometimes update texture coordinates
  // m_vertex_buffer = directx->CreateBuffer(D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, &vertices[0], sizeof(vertices));

  Vertex vertices[4] = {
    {{0, 0}, {0, 1}},
    {{0, 1}, {0, 0}},
    {{1, 1}, {1, 0}},
    {{1, 0}, {1, 1}}
  };

  int indices[] = {0, 2, 1, 0, 3, 2}; // RH
  // int indices[] = {0, 1, 2, 0, 2, 3}; // LH

  index_buffer = directx->CreateBuffer(D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, 0, &indices[0], sizeof(indices));

  vertex_buffer = directx->CreateBuffer(D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, &vertices[0], sizeof(vertices));
}