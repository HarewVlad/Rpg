void Directx::Initialize(HWND hwnd, int width, int height) {
  DXGI_SWAP_CHAIN_DESC scd = {};
  scd.BufferCount = 2;
  scd.BufferDesc.Width = 0;
  scd.BufferDesc.Height = 0;
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  scd.BufferDesc.RefreshRate.Numerator = 60;
  scd.BufferDesc.RefreshRate.Denominator = 1;
  scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scd.OutputWindow = hwnd;
  scd.SampleDesc.Count = 1;
  scd.SampleDesc.Quality = 0;
  scd.Windowed = TRUE;
  scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT create_device_flags = 0;
  D3D_FEATURE_LEVEL feature_level;
  const D3D_FEATURE_LEVEL feature_levels[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  
  assert(D3D11CreateDeviceAndSwapChain(
             NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
             feature_levels, 2, D3D11_SDK_VERSION, &scd, &swap_chain,
             &device, &feature_level,
             &device_context) == S_OK);

  // Sampler
  D3D11_SAMPLER_DESC sd = {};
  sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sd.MinLOD = 0;
  sd.MaxLOD = D3D11_FLOAT32_MAX;
  assert(device->CreateSamplerState(&sd, &sampler) == S_OK);

  // Viewport
  viewport.Width = (FLOAT)width;
  viewport.Height = (FLOAT)height;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;

  // Depth stencil state
  D3D11_DEPTH_STENCIL_DESC dssd = {};
  dssd.DepthEnable = true;
  dssd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dssd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
  dssd.StencilEnable = true;
  dssd.StencilReadMask = 0xFF;
  dssd.StencilWriteMask = 0xFF;

  dssd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dssd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  dssd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  dssd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  dssd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dssd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  dssd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  dssd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  assert(device->CreateDepthStencilState(&dssd, &depth_stencil_state) == S_OK);

  // Blend state
  D3D11_BLEND_DESC bsd = {};
  bsd.RenderTarget[0].BlendEnable = TRUE;
  bsd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  bsd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  bsd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  bsd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
  bsd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
  bsd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  bsd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

  assert(device->CreateBlendState(&bsd, &blend_state) == S_OK);

  CreateRenderTarget();
  CreateDepthStencilView(width, height);
}

void Directx::CreateRenderTarget() {
  ID3D11Texture2D *back_buffer;
  swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
  device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
  back_buffer->Release();
}

void Directx::CreateDepthStencilView(int width, int height) {
  D3D11_TEXTURE2D_DESC depth_stencil_desc = {};
  depth_stencil_desc.Width = width;
  depth_stencil_desc.Height = height;
  depth_stencil_desc.MipLevels = 1;
  depth_stencil_desc.ArraySize = 1;
  depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depth_stencil_desc.SampleDesc.Count = 1;
  depth_stencil_desc.SampleDesc.Quality = 0;
  depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
  depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depth_stencil_desc.CPUAccessFlags = 0;
  depth_stencil_desc.MiscFlags = 0;

  assert(device->CreateTexture2D(&depth_stencil_desc, nullptr, &depth_stencil) == S_OK);
  assert(device->CreateDepthStencilView(depth_stencil, nullptr, &depth_stencil_view) == S_OK);
}

void Directx::RenderBegin() {
  const float clear_color[] = {0.1, 0.1, 0.2, 1};
  device_context->ClearRenderTargetView(render_target_view, clear_color);
  device_context->ClearDepthStencilView(depth_stencil_view,
                                                D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                                1.0f, 0);
  device_context->RSSetViewports(1, &viewport);
  device_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
  device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Used to rendering opaque textures
  device_context->OMSetDepthStencilState(depth_stencil_state, 0);
  const float blend_factor[] = {0, 0, 0, 0};
  device_context->OMSetBlendState(blend_state, blend_factor, 0xffffffff);
}

void Directx::RenderEnd() {
  swap_chain->Present(0, 0);
}

ID3D11ShaderResourceView *Directx::CreateTexture(const char *filename, int *width, int *height) {
  ID3D11ShaderResourceView *result = nullptr;

  int bpp;
  unsigned char *pixels = stbi_load(filename, width, height, &bpp, 4);

  D3D11_TEXTURE2D_DESC td = {};
  td.Width = *width;
  td.Height = *height;
  td.MipLevels = 1;
  td.ArraySize = 1;
  td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  td.SampleDesc.Count = 1;
  td.Usage = D3D11_USAGE_DEFAULT;
  td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  td.CPUAccessFlags = 0;

  ID3D11Texture2D *texture = nullptr;
  D3D11_SUBRESOURCE_DATA sd;
  sd.pSysMem = pixels;
  sd.SysMemPitch = td.Width * 4;
  sd.SysMemSlicePitch = 0;
  assert(device->CreateTexture2D(&td, &sd, &texture) == S_OK);

  D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
  srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvd.Texture2D.MipLevels = td.MipLevels;
  srvd.Texture2D.MostDetailedMip = 0;
  assert(device->CreateShaderResourceView(texture, &srvd, &result) == S_OK);

  texture->Release();
  return result;
}