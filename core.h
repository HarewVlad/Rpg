enum CoreState {
  Core_None = 0,
  Core_Menu = 1 << 0,
  Core_Game = 1 << 1,
  Core_CreateMap = 1 << 2,
};

struct Core {
  Window window;
  Directx directx;
  Config config;
  TilePalette tile_palette;
  Input input;
  MapEditor map_editor;

  HashMap<char *, Image> *images = NULL;

  int state = Core_Menu;

  void Initialize(HINSTANCE instance);
  void Run();
  void Uninitialize();

private:
  void RenderMenuInterface();
  void RenderTilePaletteInterface(bool *show, int grid_step); // Need to sync map grid and tile palette grid
  void RenderCreateMapInterface();
  void Input(float dt);
  void Update(float dt);
  void Render();
  void InitializeConfig();
};