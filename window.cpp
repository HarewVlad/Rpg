extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM w, LPARAM l);

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, w, l))
    return true;

  auto data = (Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  switch (msg) {
  case WM_SIZE: {
    // Currently fullscreen only
  }
    return 0;
  case WM_SYSCOMMAND:
    if ((w & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
      return 0;
    break;
  case WM_SETFOCUS:
    Utils::FlagAdd(&data->flags, Window_Active);
    break;
  case WM_KILLFOCUS:
    Utils::FlagRemove(&data->flags, Window_Active);
    break;
  case WM_KEYDOWN:
    data->keys[w & 0xFF] = true;
    break;
  case WM_KEYUP:
    data->keys[w & 0xFF] = false;
    break;
  case WM_LBUTTONDOWN:
    data->keys[MK_LBUTTON] = true;
    break;
  case WM_LBUTTONUP:
    data->keys[MK_LBUTTON] = false;
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd, msg, w, l);
}

void Window::Initialize(const char *name, int width, int height) {
  WNDCLASSEX wc = {sizeof(WNDCLASSEX),
                   CS_CLASSDC,
                   WndProc,
                   0L,
                   0L,
                   GetModuleHandle(NULL),
                   NULL,
                   NULL,
                   NULL,
                   NULL,
                   name,
                   NULL};
  RegisterClassExA(&wc);

  hwnd = CreateWindowA(wc.lpszClassName, name, WS_POPUP, 0, 0, width, height, NULL, NULL, wc.hInstance, NULL);
  assert(hwnd != NULL);

  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
}

bool Window::IsKeyDown(char key) {
  return keys[key];
}

void Window::Show() {
  ShowWindow(hwnd, SW_SHOWDEFAULT);
  UpdateWindow(hwnd);
}