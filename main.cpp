#include "main.h"

#include "include/imgui/imgui.cpp"
#include "include/imgui/imgui_demo.cpp"
#include "include/imgui/imgui_draw.cpp"
#include "include/imgui/imgui_tables.cpp"
#include "include/imgui/imgui_widgets.cpp"
#include "include/imgui/imgui_impl_win32.cpp"
#include "include/imgui/imgui_impl_dx11.cpp"

#include "utils.cpp"
#include "config.cpp"
#include "window.cpp"
#include "directx.cpp"
#include "interface.cpp"
#include "map_editor.cpp"
#include "animationee.cpp"
#include "mesh.cpp"
#include "camera.cpp"
#include "core.cpp"

#include "test.cpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd) {
  Test::TestConfig();

  Core core;
  core.Initialize(hInstance);
  core.Run();

  return 0;
}