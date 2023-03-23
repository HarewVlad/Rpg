static void InitializeInterface(HWND hwnd) {
  ImGui::CreateContext();
  ImGui_ImplWin32_Init(hwnd);
}

static void InterfaceBegin() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
}

static void InterfaceEnd() {
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

namespace ImGui {
  void DrawGrid(const ImVec2 &p0, const ImVec2 &p1, const ImVec2 &size, float step) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRect(p0, p1, IM_COL32(255, 255, 255, 255));
    draw_list->PushClipRect(p0, p1, true);
    for (float x = step; x < size.x; x += step)
      draw_list->AddLine(ImVec2(p0.x + x, p0.y), ImVec2(p0.x + x, p1.y), IM_COL32(200, 200, 200, 200));
    for (float y = step; y < size.y; y += step)
      draw_list->AddLine(ImVec2(p0.x, p0.y + y), ImVec2(p1.x, p0.y + y), IM_COL32(200, 200, 200, 200));
    draw_list->PopClipRect();
  }

  void ListBox(const char *label, int *index, const int *data, size_t size) {
    if (ImGui::BeginListBox(label)) {
      for (int i = 0; i < size; i++) {
        const bool is_selected = (*index == i);
        
        char buffer[32];
        sprintf(buffer, "##ListBox##%d", i);
        if (ImGui::Selectable(buffer, is_selected))
          *index = i;

        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndListBox();
    }
  }
};