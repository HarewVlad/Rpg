void Core::InitializeConfig() {
  config.Put<int>("fps", 144);
  config.Put<int>("interface_speed", 3);
}

void Core::Initialize(HINSTANCE instance) {
  InitializeConfig();

  window.Initialize(Constants::Name, Constants::Width, Constants::Height);
  InitializeInterface(window.hwnd);

  directx.Initialize(window.hwnd, Constants::Width, Constants::Height);
  ImGui_ImplDX11_Init(directx.device, directx.device_context);

  window.Show();
}

void Core::Run() {
  int old_time = Utils::GetMilliseconds();
  int extra_time = 0;
  int frame_time = 1000 / config.Get<int>("fps");
  float dt = frame_time / 1000.0f;

  MSG msg = {};
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    int new_time = Utils::GetMilliseconds();
    int time = new_time - old_time;

    /////// FRAME ///////

    bool is_active = Utils::FlagCheck(window.flags, Window_Active);
    if (is_active) {
      extra_time += time;

      while (extra_time >= frame_time) {
        Input(dt);
        Update(dt);

        extra_time -= frame_time;
      }

      Render();
    } else {
      Sleep(5);
    }

    /////// END FRAME ///////

    old_time = new_time;
  }
}

void Core::Input(float dt) {
  int speed = config.Get<int>("interface_speed");
  if (window.IsKeyDown('W'))
    input.offset.y += speed;
  if (window.IsKeyDown('A'))
    input.offset.x += speed;
  if (window.IsKeyDown('S'))
    input.offset.y -= speed;
  if (window.IsKeyDown('D'))
    input.offset.x -= speed;
}

void Core::Update(float dt) {

}

void Core::RenderAnimationeeInterface(bool *show) {
  ImGui::Begin("Animationee", show, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        const char *filenames = Utils::SelectFiles();
        defer { arrfree(filenames); };

        if (filenames != NULL) {
          const char *directory = filenames; // Directory stored first
          for (int i = 0; i < arrlen(filenames); i++) {
            if (filenames[i] == '\0') {
              const char *file = filenames + i + 1;
              if (*file != '\0') {
                char *filename = NULL;
                arrsetlen(filename, 256);
                memset(filename, 0, arrlen(filename));

                sprintf(filename, "%s\\%s", directory, file);

                Image image;
                image.Initialize(&directx, filename);
                shput(animationee_images, filename, image);
              } else {
                break;
              }
            }
          }
        }
      }
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }

  for (int i = 0; i < shlen(animationee_images); i++) {
    const auto image = animationee_images[i].value;

    if (ImGui::ImageButton((void *)image.texture, image.size)) {

    }
    ImGui::SameLine();
  }

  ImGui::End();
}

void Core::RenderTilePaletteInterface(bool *show, int grid_step) {
  ImGui::Begin("Tile palette", show, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

  static int zoom = 1;
  const int zoomed_grid_step = grid_step * zoom;

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        const char *filename = Utils::SelectFile();
        if (filename != NULL && shgeti(tile_palette_images, filename) < 0) {
          Image image;
          image.Initialize(&directx, filename);
          shput(tile_palette_images, filename, image);
        }
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Options")) {
      ImGui::SliderInt("Zoom", &zoom, 1, 10);
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }

  ImGui::BeginTabBar("Images");
  for (int i = 0; i < shlen(tile_palette_images); i++) {
    const auto key = tile_palette_images[i].key;
    const auto image = tile_palette_images[i].value;

    if (ImGui::BeginTabItem(key)) {
      tile_palette.SetActive(key);

      const ImVec2 p0 = ImGui::GetCursorScreenPos();
      const ImVec2 origin = ImGui::GetCursorPos();
      const ImVec2 size = image.size * zoom;
      const ImVec2 p1 = p0 + size;
      ImGui::DrawGrid(p0, p1, size, zoomed_grid_step);

      ImGui::Image((void *)image.texture, size);

      ImGui::SetCursorPos(origin);
      ImGui::InvisibleButton("Image", size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
      const bool is_active = ImGui::IsItemActive();
      const bool is_hovered = ImGui::IsItemHovered();

      ImGuiIO &io = ImGui::GetIO();
      const ImVec2 mouse_position = io.MousePos - p0;
      const IVec2 mouse_tile = mouse_position / zoomed_grid_step;
      const IVec2 max_tiles = image.size / grid_step;

      static bool is_selecting = false;
      if (is_hovered && !is_selecting && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        tile_palette.Clear();
        tile_palette.start_tile = mouse_tile;
        is_selecting = true;
      }

      if (is_selecting) {
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
          tile_palette.AddTiles();
          is_selecting = false;
        }
      }

      if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0)) {
        if (mouse_tile.x < max_tiles.x && mouse_tile.y < max_tiles.y)
          tile_palette.end_tile = mouse_tile;
      }

      if (is_active && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        tile_palette.Clear();

      for (int j = 0; j < hmlen(tile_palette.active_tiles); j++) {
        const auto active_tile = tile_palette.active_tiles[j].key;
        const ImVec2 start = p0 + active_tile * zoomed_grid_step;
        const ImVec2 end = ImVec2(start.x + zoomed_grid_step, start.y + zoomed_grid_step);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(start, end, IM_COL32(255, 255, 255, 128));
      }

      ImGui::EndTabItem();
    }
  }

  ImGui::EndTabBar();
  ImGui::End();
}

void Core::RenderCreateMapInterface() {
  static bool show_tile_palette = false;
  static bool show_animationee = false;
  static bool show_grid = true;
  static int grid_step = 32;
  static int zoom = 1;
  static IVec2 map_size = IVec2(512, 512);
  const int zoomed_grid_step = grid_step * zoom;
  const IVec2 max_tiles = map_size / zoomed_grid_step;

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30);
  ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f));
  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.0f, 0.0f, 1.0f));
  ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);

  ImGui::SetCursorPosY(25); // To avoid menu bar

  if (ImGui::Button("Tile palette"))
    show_tile_palette = true;

  if (ImGui::Button("Animationee"))
    show_animationee = true;

  if (show_tile_palette) RenderTilePaletteInterface(&show_tile_palette, grid_step);
  if (show_animationee) RenderAnimationeeInterface(&show_animationee);

  ImGui::PushItemWidth(150);
  ImGui::Checkbox("Show grid", &show_grid);
  ImGui::DragInt("Grid step", &grid_step, 8, 8, 64, "%d", ImGuiSliderFlags_AlwaysClamp);
  ImGui::SliderInt("Zoom", &zoom, 1, 10);
  ImGui::InputInt("Width", &map_size.x);
  ImGui::InputInt("Height", &map_size.y);
  ImGui::PopItemWidth();

  if (ImGui::Button("Clear map"))
    map_editor.Clear();

  if (ImGui::Button("Save map")) {
    const char *filename = Utils::SaveFile();
    if (filename != NULL) {
      map_editor.Save(filename);
    }
  }

  if (ImGui::Button("Load map")) {
    const char *filename = Utils::SelectFile();
    if (filename != NULL) {
      map_editor.Clear();
      map_editor.Load(filename);
    }

    for (int i = 0; i < hmlen(map_editor.elements); i++) {
      const char *key = map_editor.elements[i].key;

      if (shgeti(tile_palette_images, key) < 0) {
        Image image;
        image.Initialize(&directx, key);
        shput(tile_palette_images, key, image);
      }
    }
  }

  if (ImGui::Button("Exit"))
    state = Core_Menu;

  const ImVec2 p0 = input.offset;
  const ImVec2 p1 = p0 + map_size * zoom;
  if (show_grid) ImGui::DrawGrid(p0, p1, map_size * zoom, zoomed_grid_step);

  ImGui::SetCursorPos(p0 - ImVec2(0, 25));
  static bool is_adding_player = false;
  if (ImGui::Button("Player")) {
    is_adding_player = true;
  }

  ImGui::SameLine();
  if (ImGui::Button("Wall")) {

  }

  ImGui::SetCursorPos(p0);
  ImGui::InvisibleButton("Image", map_size * zoom, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
  const bool is_active = ImGui::IsItemActive();
  const bool is_hovered = ImGui::IsItemHovered();

  const ImVec2 mouse_position = ImGui::GetIO().MousePos - p0;
  const IVec2 mouse_tile = mouse_position / zoomed_grid_step;

  if (is_active && is_adding_player && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    map_editor.player_position = mouse_tile;
    is_adding_player = false;
  }

  if (tile_palette.active_key != NULL) {
    const auto active_image = shget(tile_palette_images, tile_palette.active_key);
    for (int i = 0; i < hmlen(tile_palette.active_tiles); i++) {
      const auto start_tile = tile_palette.active_tiles[0].key;
      const auto active_tile = tile_palette.active_tiles[i].key;
      const IVec2 tile_offset = active_tile - (start_tile - mouse_tile);

      ImVec2 tile_uv0 = active_tile * grid_step;
      ImVec2 tile_uv1 = ImVec2(tile_uv0.x + grid_step, tile_uv0.y + grid_step);
      tile_uv0 /= active_image.size;
      tile_uv1 /= active_image.size;

      if (tile_offset.x < max_tiles.x && tile_offset.y < max_tiles.y &&
          tile_offset.x >= 0 && tile_offset.y >= 0) {
        ImGui::SetCursorPos(p0 + tile_offset * zoomed_grid_step);
        ImGui::Image((void *)active_image.texture, ImVec2(zoomed_grid_step, zoomed_grid_step),
                     tile_uv0, tile_uv1, ImVec4(1, 1, 1, 0.5f));

        if (is_active && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
          map_editor.AddElement(tile_palette.active_key, tile_uv0, tile_uv1, tile_offset);
      }
    }
  }

  if (is_active && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    map_editor.Remove(mouse_tile);
  
  for (int i = 0; i < shlen(map_editor.elements); i++) {
    const auto key = map_editor.elements[i].key;
    const auto map = map_editor.elements[i].value;
    const auto image = shget(tile_palette_images, key);
    for (int j = 0; j < hmlen(map); j++) {
      const auto position = map[j].key;
      const auto array = map[j].value;
      for (int k = 0; k < arrlen(array); k++) {
        const auto element = array[k];

        ImGui::SetCursorPos(p0 + position * zoomed_grid_step);
        ImGui::Image((void *)image.texture, ImVec2(zoomed_grid_step, zoomed_grid_step), element.uv0, element.uv1);
      }
    }
  }

  if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
    map_editor.Undo();
  
  ImGui::PopTextWrapPos();
  ImGui::PopStyleColor(2);
  ImGui::PopStyleVar();
}

void Core::RenderMenuInterface() {
  ImGuiIO &io = ImGui::GetIO();

  static const ImVec2 button_size = {200, 100};

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30);
  ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f));
  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.0f, 0.0f, 1.0f));
  ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);

  ImGui::SetCursorPos({io.DisplaySize.x / 2.0f - button_size.x * 0.5f, io.DisplaySize.y / 2.0f - button_size.y * 3});
  if (ImGui::Button("Play", button_size))
    state = Core_Game;

  ImGui::SetCursorPosX(io.DisplaySize.x / 2.0f - button_size.x * 0.5f);
  if (ImGui::Button("Create map", button_size))
    state = Core_CreateMap;

  ImGui::SetCursorPosX(io.DisplaySize.x / 2.0f - button_size.x * 0.5f);
  ImGui::Button("About", button_size);
  ImGui::SetCursorPosX(io.DisplaySize.x / 2.0f - button_size.x * 0.5f);
  if (ImGui::Button("Exit", button_size))
    PostQuitMessage(0);

  ImGui::PopTextWrapPos();
  ImGui::PopStyleColor(2);
  ImGui::PopStyleVar();
}

void Core::Render() {
  directx.RenderBegin();
  InterfaceBegin();

  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
  ImGui::SetNextWindowSize(io.DisplaySize);
  ImGui::SetNextWindowBgAlpha(0);

  // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
  ImGui::Begin("Interface", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
               ImGuiWindowFlags_NoBringToFrontOnFocus |
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
               ImGuiWindowFlags_NoScrollbar);

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Menu")) {
      if (ImGui::MenuItem("Config")) {

      }
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  switch (state) {
    case Core_Menu:
      RenderMenuInterface();
    break;
    case Core_CreateMap:
      RenderCreateMapInterface();
    break;
  }

  bool show_demo_window = true;
  ImGui::ShowDemoWindow(&show_demo_window);

  // ImGui::PopStyleVar(2);
  ImGui::End();

  InterfaceEnd();
  directx.RenderEnd();
}