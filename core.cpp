void Core::InitializeConfig() {
  config.Put<int>("fps", 144);
  config.Put<int>("interface_speed", 3);
}

void Core::Initialize(HINSTANCE instance) {
  InitializeConfig();

  window.Initialize(Constants::Name, Constants::Width, Constants::Height);

  ImGui::CreateContext();
  ImGui_ImplWin32_Init(window.hwnd);

  directx.Initialize(window.hwnd, Constants::Width, Constants::Height);
  ImGui_ImplDX11_Init(directx.device, directx.device_context);

  map_vertex_shader = directx.CreateVertexShader(L"shaders\\map_vs.hlsl");
  map_pixel_shader = directx.CreatePixelShader(L"shaders\\map_ps.hlsl");

  D3D11_INPUT_ELEMENT_DESC map_input_element_desc[] = {
    {"position", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
     D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
     D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  map_input_layout = directx.CreateInputLayout(map_input_element_desc,
                                               _countof(map_input_element_desc),
                                               &map_vertex_shader);

  constant_buffer = directx.CreateBuffer(D3D11_USAGE_DYNAMIC, 
                                         D3D11_BIND_CONSTANT_BUFFER,
                                         D3D11_CPU_ACCESS_WRITE,
                                         NULL, sizeof(ConstantBuffer));

  mesh.Initialize(&directx);

  camera.position = {0, 0};

  projection = XMMatrixOrthographicOffCenterRH(0, Constants::Width, -Constants::Height, 0, 0.01f, 1024);

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
  if (window.IsKeyDown('W')) {
    camera.position.y += speed;
    interface_offset.y += speed;
  }
  if (window.IsKeyDown('A')) {
    camera.position.x += speed;
    interface_offset.x += speed;
  }
  if (window.IsKeyDown('S')) {
    camera.position.y -= speed;
    interface_offset.y -= speed;
  }
  if (window.IsKeyDown('D')) {
    camera.position.x -= speed;
    interface_offset.x -= speed;
  }
}

void Core::Update(float dt) {
  for (int i = 0; i < arrlen(animationee.animations); i++) {
    animationee.animations[i].Update(dt);
  }
}

void Core::RenderAnimationeeInterface(bool *show, bool *add_animation) {
  ImGui::Begin("Animationee", show, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        const char *filenames = Utils::SelectFiles();
        defer { arrfree(filenames); };

        if (filenames != NULL) {
          const char *directory = filenames; // Directory stored first

          Animation animation;

          for (int i = 0; i < arrlen(filenames); i++) {
            if (filenames[i] == '\0') {
              const char *file = filenames + i + 1;

              if (*file != '\0') {
                char *filename = Utils::GetString(MAX_PATH);
                sprintf(filename, "%s\\%s", directory, file);

                if (shgeti(animationee.images, filename) < 0) {
                  const auto image = directx.CreateImage(filename);
                  shput(animationee.images, filename, image); 
                }

                arrput(animation.filenames, filename);
              } else {
                break;
              }
            }
          }

          arrput(animationee.animations, animation);
        }
      }
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }

  for (int i = 0; i < arrlen(animationee.animations); i++) {
    auto &animation = animationee.animations[i];
    const auto filename = animation.filenames[animation.index];
    const auto image = shget(animationee.images, filename);

    ImGui::Image((void *)image.texture, image.size); ImGui::SameLine();

    char label[32];
    sprintf(label, "##Combo##%d", i);

    ImGui::Combo(label, &animation.action, Action_String, IM_ARRAYSIZE(Action_String)); ImGui::SameLine();
    if (ImGui::Button("Select")) {
      animationee.active_animation.index = i;
      animationee.active_animation.action = animation.action;
      *add_animation = true;
    }
  }

  ImGui::End();
}

void Core::RenderTilePaletteInterface(bool *show) {
  ImGui::Begin("Tile palette", show, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

  static int zoom = 1;
  const int grid_step = map_editor.GetGridStep();
  const int zoomed_grid_step = grid_step * zoom;

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        const char *filename = Utils::SelectFile();

        if (filename != NULL && shgeti(map_editor.images, filename) < 0) {
          const auto image = directx.CreateImage(filename);
          shput(map_editor.images, filename, image);
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
  for (int i = 0; i < shlen(map_editor.images); i++) {
    const auto filename = map_editor.images[i].key;
    const auto image = map_editor.images[i].value;

    if (ImGui::BeginTabItem(filename)) {
      if (map_editor.active_filename != filename) {
        map_editor.active_filename = filename;
        hmfree(map_editor.active_tiles);
      }

      const ImVec2 p0 = ImGui::GetCursorScreenPos();
      const ImVec2 origin = ImGui::GetCursorPos();
      const ImVec2 size = image.size * zoom;
      const ImVec2 p1 = p0 + size;
      ImGui::DrawGrid(p0, p1, size, zoomed_grid_step);

      ImGui::Image((void *)image.texture, size);

      ImGui::SetCursorPos(origin);
      ImGui::InvisibleButton("Image", size,
                             ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
      const bool is_active = ImGui::IsItemActive();
      const bool is_hovered = ImGui::IsItemHovered();

      ImGuiIO &io = ImGui::GetIO();
      const ImVec2 mouse_position = io.MousePos - p0;
      const IVec2 mouse_tile = mouse_position / zoomed_grid_step;
      const IVec2 max_tiles = image.size / grid_step;

      static IVec2 start_tile;
      static IVec2 end_tile;
      static bool is_selecting = false;
      if (is_hovered && !is_selecting && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        hmfree(map_editor.active_tiles);
        start_tile = mouse_tile;
        is_selecting = true;
      }

      if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0)) {
        if (mouse_tile.x < max_tiles.x && mouse_tile.y < max_tiles.y)
          end_tile = mouse_tile;
      }

      if (is_selecting) {
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {

          for (int y = start_tile.y; y <= end_tile.y; y++) {
            for (int x = start_tile.x; x <= end_tile.x; x++) {
              IVec2 tile = ImVec2(x, y);
              hmput(map_editor.active_tiles, tile, 1);
            }
          }

          is_selecting = false;
        }
      }

      if (is_active && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        hmfree(map_editor.active_tiles);

      for (int j = 0; j < hmlen(map_editor.active_tiles); j++) {
        const auto active_tile = map_editor.active_tiles[j].key;
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
  static bool add_animation = false;
  static bool add_character = false;
  static int zoom = 1;
  const int grid_step = map_editor.GetGridStep();
  const int zoomed_grid_step = grid_step * zoom;
  const IVec2 max_tiles = map_editor.size / zoomed_grid_step;

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30);
  ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f));
  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.0f, 0.0f, 1.0f));
  ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);

  ImGui::SetCursorPosY(25); // To avoid menu bar

  if (ImGui::Button("Tile palette"))
    show_tile_palette = true;

  if (ImGui::Button("Animationee"))
    show_animationee = true;

  ImGui::PushItemWidth(150);
  ImGui::Checkbox("Show grid", &show_grid);
  ImGui::Combo("Grid step", &map_editor.grid_step_index, map_editor.grid_steps_string,
               IM_ARRAYSIZE(map_editor.grid_steps_string));
  ImGui::SliderInt("Zoom", &zoom, 1, 10);
  ImGui::InputInt("Width", &map_editor.size.x);
  ImGui::InputInt("Height", &map_editor.size.y);
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
      filename = map_editor.elements[i].key;

      if (shgeti(map_editor.images, filename) < 0) {
        const auto image = directx.CreateImage(filename);
        shput(map_editor.images, filename, image);
      }
    }
  }

  if (ImGui::Button("Test map")) {
    // if (map_instance_buffer) map_instance_buffer->Release();
    // map_instance_buffer = directx.CreateBuffer(D3D11_USAGE_DYNAMIC, 
    //                                            D3D11_BIND_VERTEX_BUFFER,
    //                                            D3D11_CPU_ACCESS_WRITE, NULL, 
    //                                            map_editor.size.x * map_editor.size.y);

    state = Core_TestMap;
  }

  if (ImGui::Button("Exit"))
    state = Core_Menu;

  if (show_tile_palette) RenderTilePaletteInterface(&show_tile_palette);
  if (show_animationee) RenderAnimationeeInterface(&show_animationee, &add_animation);

  const ImVec2 p0 = interface_offset;
  const ImVec2 p1 = p0 + map_editor.size * zoom;
  if (show_grid) ImGui::DrawGrid(p0, p1, map_editor.size * zoom, zoomed_grid_step);

  ImGui::SetCursorPos(p0 - ImVec2(0, 25));
  ImGui::Checkbox("Character", &add_character);

  ImGui::SameLine();
  if (ImGui::Button("Wall")) {

  }

  ImGui::SetCursorPos(p0);
  ImGui::InvisibleButton("Image", map_editor.size * zoom,
                         ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
  const bool is_active = ImGui::IsItemActive();
  const bool is_hovered = ImGui::IsItemHovered();

  const ImVec2 mouse_position = ImGui::GetIO().MousePos - p0;
  const IVec2 mouse_tile = mouse_position / zoomed_grid_step;

  if (is_active && add_animation && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    if (hmgeti(map_editor.characters, mouse_tile) >= 0) {
      auto &character = hmget(map_editor.characters, mouse_tile);
      character.animations[animationee.active_animation.index] = animationee.active_animation.action;
      add_animation = false;    
    }
  }

  if (is_active && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    map_editor.Remove(mouse_tile);

  if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
    map_editor.Undo();

  if (is_active && add_character && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    map_editor.AddCharacter(mouse_tile);

  if (add_character && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    hmdel(map_editor.characters, mouse_tile);

  if (map_editor.active_filename != NULL) {
    const auto active_image = shget(map_editor.images, map_editor.active_filename);

    for (int i = 0; i < hmlen(map_editor.active_tiles); i++) {
      const auto start_tile = map_editor.active_tiles[0].key;
      const auto active_tile = map_editor.active_tiles[i].key;
      const IVec2 tile_offset = active_tile - (start_tile - mouse_tile);

      ImVec2 tile_uv0 = active_tile * grid_step;
      ImVec2 tile_uv1 = ImVec2(tile_uv0.x + grid_step, tile_uv0.y + grid_step);
      tile_uv0 /= active_image.size;
      tile_uv1 /= active_image.size;

      ImGui::SetCursorPos(p0 + tile_offset * zoomed_grid_step);
      ImGui::Image((void *)active_image.texture, ImVec2(zoomed_grid_step, zoomed_grid_step),
                   tile_uv0, tile_uv1, ImVec4(1, 1, 1, 0.5f));

      if (is_active && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        map_editor.AddElement(map_editor.active_filename, tile_uv0, tile_uv1, tile_offset);
    }
  }
  
  for (int i = 0; i < shlen(map_editor.elements); i++) {
    const auto filename = map_editor.elements[i].key;
    const auto map = map_editor.elements[i].value;
    const auto image = shget(map_editor.images, filename);

    for (int j = 0; j < hmlen(map); j++) {
      const auto position = map[j].key;
      const auto array = map[j].value;

      for (int k = 0; k < arrlen(array); k++) {
        const auto element = array[k];

        ImGui::SetCursorPos(p0 + position * zoomed_grid_step);
        ImGui::Image((void *)image.texture, 
                     ImVec2(zoomed_grid_step, zoomed_grid_step), element.uv0, element.uv1);
      }
    }
  }

  for (int i = 0; i < hmlen(map_editor.characters); i++) {
    const auto position = map_editor.characters[i].key;
    const auto character = map_editor.characters[i].value;

    const ImVec2 start = p0 + position * zoomed_grid_step;
    const ImVec2 end = ImVec2(start.x + zoomed_grid_step, start.y + zoomed_grid_step);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(start, end, IM_COL32(128, 255, 128, 128));
  }
  
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

  ImGui::SetCursorPos({io.DisplaySize.x / 2.0f - button_size.x * 0.5f,
                       io.DisplaySize.y / 2.0f - button_size.y * 3});
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

void Core::RenderTestMap() {
  const auto device_context = directx.device_context;

  device_context->IASetInputLayout(map_input_layout);
  device_context->VSSetShader(map_vertex_shader.shader, nullptr, 0);
  device_context->VSSetConstantBuffers(0, 1, &constant_buffer);
  device_context->PSSetShader(map_pixel_shader.shader, nullptr, 0);

  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  device_context->IASetVertexBuffers(0, 1, &mesh.vertex_buffer, &stride, &offset);
  device_context->IASetIndexBuffer(mesh.index_buffer, DXGI_FORMAT_R32_UINT, 0);

  const auto view = camera.GetView();
  const auto grid_step = map_editor.GetGridStep();
  const auto scale = XMMatrixScaling(grid_step, grid_step, 1);
  
  for (int i = 0; i < shlen(map_editor.elements); i++) {
    const auto key = map_editor.elements[i].key;
    const auto hash_map = map_editor.elements[i].value;
    const auto image = shget(map_editor.images, key);

    device_context->PSSetShaderResources(0, 1, &image.texture);

    for (int j = 0; j < hmlen(hash_map); j++) {
      const auto position = hash_map[j].key;
      const auto array = hash_map[j].value;

      for (int k = 0; k < arrlen(array); k++) {
        const auto element = array[k];

        const auto model = scale *
                           XMMatrixTranslation(-position.x * grid_step - grid_step,
                                               -position.y * grid_step - grid_step, 0);
        const auto mvp = model * view * projection;

        ConstantBuffer data = { XMMatrixTranspose(mvp) };
        directx.UpdateBuffer(constant_buffer, &data, sizeof(data));

        Vertex vertices[4] = {
          {{0, 0}, element.uv1},
          {{0, 1}, {element.uv1.x, element.uv0.y}},
          {{1, 1}, element.uv0},
          {{1, 0}, {element.uv0.x, element.uv1.y}}
        };
        directx.UpdateBuffer(mesh.vertex_buffer, &vertices, sizeof(vertices));

        device_context->DrawIndexed(6, 0, 0);
      }
    }
  }

  for (int i = 0; i < hmlen(map_editor.characters); i++) {
    const auto position = map_editor.characters[i].key;
    const auto character = map_editor.characters[i].value;
    const auto animation_index = character.animations[character.current_animation];
    const auto animation = animationee.animations[animation_index];
    const auto image = shget(animationee.images, animation.filenames[animation.index]);

    device_context->PSSetShaderResources(0, 1, &image.texture);

    const auto model = scale *
                       XMMatrixTranslation(-position.x * grid_step - grid_step,
                                           -position.y * grid_step - grid_step, 0);
    const auto mvp = model * view * projection;

    ConstantBuffer data = { XMMatrixTranspose(mvp) };
    directx.UpdateBuffer(constant_buffer, &data, sizeof(data));

    Vertex vertices[4] = {
      {{0, 0}, {0, 1}},
      {{0, 1}, {0, 0}},
      {{1, 1}, {1, 0}},
      {{1, 0}, {1, 1}}
    };
    directx.UpdateBuffer(mesh.vertex_buffer, &vertices, sizeof(vertices));

    device_context->DrawIndexed(6, 0, 0);
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30);
  ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f));
  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.0f, 0.0f, 1.0f));
  ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);

  ImGui::SetCursorPosY(25); // To avoid menu bar

  if (ImGui::Button("Back"))
    state = Core_CreateMap;

  ImGui::PopTextWrapPos();
  ImGui::PopStyleColor(2);
  ImGui::PopStyleVar();
}

void Core::Render() {
  const auto device_context = directx.device_context;

  const float clear_color[] = {0.1, 0.1, 0.2, 1};
  device_context->ClearRenderTargetView(directx.render_target_view, clear_color);
  device_context->ClearDepthStencilView(directx.depth_stencil_view,
                                                D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                                1.0f, 0);
  device_context->RSSetViewports(1, &directx.viewport);
  device_context->OMSetRenderTargets(1, &directx.render_target_view, directx.depth_stencil_view);
  device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Used to rendering opaque textures
  device_context->OMSetDepthStencilState(directx.depth_stencil_state, 0);
  const float blend_factor[] = {0, 0, 0, 0};
  device_context->OMSetBlendState(directx.blend_state, blend_factor, 0xffffffff);

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
  ImGui::SetNextWindowSize(io.DisplaySize);
  ImGui::SetNextWindowBgAlpha(0);

  // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
  ImGui::Begin("Interface", NULL, ImGuiWindowFlags_NoMove |
               ImGuiWindowFlags_NoTitleBar |
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
    case Core_TestMap:
      RenderTestMap();
    break;
  }

  bool show_demo_window = true;
  ImGui::ShowDemoWindow(&show_demo_window);

  // ImGui::PopStyleVar(2);
  ImGui::End();
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  directx.swap_chain->Present(0, 0);
}