struct Image {
  ID3D11ShaderResourceView *texture = NULL;
  IVec2 size;

  void Initialize(Directx *directx, const char *filename);
};