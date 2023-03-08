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

  static const char *SelectFile() {
    char *filename = NULL;
    arrsetlen(filename, 256);
    memset(filename, 0, arrlen(filename));

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
      arrfree(filename);
      return NULL;
    }
  }

  static const char *SaveFile() {
    char *filename = NULL;
    arrsetlen(filename, 256);
    memset(filename, 0, arrlen(filename));

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = static_cast<DWORD>(arrlen(filename));
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetSaveFileNameA(&ofn)) {
      return filename;
    } else {
      arrfree(filename);      
      return NULL;
    }
  }

  static const char *SelectFiles() {
    char *filename = NULL;
    arrsetlen(filename, 256 * 16); // Up to 16 files
    memset(filename, 0, arrlen(filename));

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = static_cast<DWORD>(arrlen(filename));
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileNameA(&ofn)) {
      return filename;
    } else {
      arrfree(filename);      
      return NULL;
    }
  }
};

