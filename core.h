enum Core_ {
  Core_None = 0,
  Core_Menu = 1 << 0,
  Core_Game = 1 << 1,
  Core_CreateMap = 1 << 2,
  Core_TestMap = 1 << 3,
};

struct Core {
  Window window;
  Directx directx;
  Config config;
  ImVec2 interface_offset;
  MapEditor map_editor;
  Mesh mesh;
  Animationee animationee;
  VertexShader map_vertex_shader;
  PixelShader map_pixel_shader;
  ID3D11InputLayout *map_input_layout;
  ID3D11Buffer *constant_buffer;
  // ID3D11Buffer *map_instance_buffer;
  Camera camera;
  XMMATRIX projection;

  int state = Core_Menu;

  void Initialize(HINSTANCE instance);
  void Run();

private:
  void RenderTestMap();
  void RenderAnimationeeInterface(bool *show, bool *add_animation);
  void RenderMenuInterface();
  void RenderTilePaletteInterface(bool *show);
  void RenderCreateMapInterface();
  void Input(float dt);
  void Update(float dt);
  void Render();
  void InitializeConfig();
};