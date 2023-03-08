struct TilePalette {
  HashMap<IVec2, bool> *active_tiles = NULL;
  const char **keys = NULL; // Used internally to determine what images are used for tile palette
  const char *active_key = NULL;
  IVec2 start_tile;
  IVec2 end_tile;

  void AddTiles();
  void Clear();
  void SetActive(const char *key);
};