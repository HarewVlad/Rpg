struct Image {
  ID3D11ShaderResourceView *texture;
  IVec2 size;
};

struct VertexShader {
  ID3D11VertexShader *shader;
  ID3D10Blob *blob;
};

struct PixelShader {
  ID3D11PixelShader *shader;
  ID3D10Blob *blob;
};

#pragma pack(push, 16)
struct ConstantBuffer {
  XMMATRIX mvp;
};
#pragma pack(pop)

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
  Image CreateImage(const char *filename);
  VertexShader CreateVertexShader(LPCWSTR filename);
  PixelShader CreatePixelShader(LPCWSTR filename);
  ID3D11InputLayout *CreateInputLayout(D3D11_INPUT_ELEMENT_DESC *vertex_element_desc,
                                       UINT size,
                                       VertexShader *vertex_shader);
  ID3D11Buffer *CreateBuffer(D3D11_USAGE usage, UINT type, 
                             UINT cpu_flags, void *data, UINT size);

private:
  ID3D10Blob *CompileShader(LPCWSTR filename, const char *target);
  void CreateRenderTarget();
  void CreateDepthStencilView(int width, int height);
};