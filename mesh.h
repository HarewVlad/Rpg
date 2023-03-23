struct Vertex {
  ImVec2 position;
  ImVec2 uv;
};

struct Mesh {
  ID3D11Buffer *index_buffer;
  ID3D11Buffer *vertex_buffer;

  void Initialize(Directx *directx);
};