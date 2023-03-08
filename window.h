enum Window_ {
  Window_None = 0,
  Window_Active = 1 << 0,
};

struct Window {
  HWND hwnd;
  bool keys[0x100] = {};
  int flags = Window_None;

  void Initialize(const char *name, int width, int height);
  bool IsKeyDown(char key);
  void Show();
};