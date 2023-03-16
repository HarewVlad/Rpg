struct MapEditor {
  struct Element {
    ImVec2 uv0;
    ImVec2 uv1;
  };

  HashMap<char *, HashMap<IVec2, Element *> *> *elements = NULL;
  const char **keys = NULL; // Used internally for undo operation
  IVec2 player_position;

  void AddElement(const char *key, const ImVec2 &uv0, const ImVec2 &uv1, const IVec2 &position);
  void Save(const char *filename);
  void Load(const char *filename);
  void Undo();
  void Remove(const IVec2 &position);
  void Clear();
};