struct MapEditor {
  struct Element {
    ImVec2 uv0;
    ImVec2 uv1;
  };

  HashMap<char *, HashMap<IVec2, Element *> *> *elements = NULL;
  const char **filenames = NULL; // Used internally for undo operation
  HashMap<IVec2, Character> *characters = NULL;
  IVec2 size = IVec2(512, 512);
  XMMATRIX *element_model_matrices = NULL;

  void InitializeModelMatrices();
  void AddElement(const char *filename, const ImVec2 &uv0, const ImVec2 &uv1, const IVec2 &position);
  void AddCharacter(const IVec2 &position);
  void Save(const char *filename);
  void Load(const char *filename);
  void Undo();
  void Remove(const IVec2 &position);
  void Clear();
};