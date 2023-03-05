namespace Utils {
  static int GetMilliseconds() {
    static int base = timeGetTime();
    return timeGetTime() - base;
  }

  static void FlagAdd(int *flags, int flag) {
    *flags |= flag;
  }

  static void FlagRemove(int *flags, int flag) {
    *flags &= ~flag;
  }

  static bool FlagCheck(int flags, int flag) {
    return flags & flag;
  }

  static const char *OpenFileBrowser() {
    // wchar_t buffer[32767];
    // if (GetFileNameFromBrowse(hwnd, buffer, sizeof(buffer), NULL, NULL, NULL, NULL)) {
    //   if (!stbi_convert_wchar_to_utf8(filename, size, buffer)) {
    //     assert(0 && "Failed to convert wchar_t to utf8");
    //   }
    // } else {
    //   assert(0 && "Failed to open file");
    // }

    char *filename = new char[256];

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = static_cast<DWORD>(256);
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
      return filename;
    } else {
      delete filename;
      return NULL;
    }
  }
};