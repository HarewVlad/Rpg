struct Directx {
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;
  IDXGISwapChain *swap_chain;
  ID3D11RenderTargetView *render_target_view;
  ID3D11Texture2D *depth_stencil;
  ID3D11DepthStencilView *depth_stencil_view;
  ID3D11SamplerState *sampler;
  D3D11_VIEWPORT viewport;
  ID3D11DepthStencilState *depth_stencil_state;
  ID3D11BlendState *blend_state;

  void Initialize(HWND hwnd, int width, int height);
  void RenderBegin();
  void RenderEnd();
  ID3D11ShaderResourceView *CreateTexture(const char *filename, int *width, int *height);

private:
  void Directx::CreateRenderTarget();
  void Directx::CreateDepthStencilView(int width, int height);
};