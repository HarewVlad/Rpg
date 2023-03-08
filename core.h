enum Core_ {
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
  Animationee animationee;

  HashMap<char *, Image> *tile_palette_images = NULL;
  HashMap<char *, Image> *animationee_images = NULL;

  int state = Core_Menu;

  void Initialize(HINSTANCE instance);
  void Run();
  void Uninitialize();

private:
  void RenderAnimationeeInterface(bool *show);
  void RenderMenuInterface();
  void RenderTilePaletteInterface(bool *show, int grid_step); // Need to sync map grid and tile palette grid
  void RenderCreateMapInterface();
  void Input(float dt);
  void Update(float dt);
  void Render();
  void InitializeConfig();
};