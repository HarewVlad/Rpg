void MapEditor::AddElement(const char *key, const ImVec2 &uv0, const ImVec2 &uv1, const IVec2 &position) {
  auto hash_map = shget(elements, key);
  hmput(hash_map, position, (Element{uv0, uv1}));
  shput(elements, key, hash_map);
  arrput(keys, key);
}

void MapEditor::Clear() {
  for (int i = 0; i < shlen(elements); i++) {
    auto hash_map = elements[i].value;
    hmfree(hash_map);
  }
  hmfree(elements);
}

void MapEditor::Save() {
  std::ofstream out("map.txt", std::ofstream::trunc);
  defer { out.close(); };

  if (out) {
    for (int i = 0; i < shlen(elements); i++) {
      const auto key = elements[i].key;
      const auto hash_map = elements[i].value;

      out << key << ' ' << hmlen(hash_map) << ' ';

      for (int j = 0; j < hmlen(hash_map); j++) {
        const auto position = hash_map[j].key;
        const auto element = hash_map[j].value;

        out << position.x << ' ' << position.y << ' '
            << element.uv0.x << ' ' << element.uv0.y << ' '
            << element.uv1.x << ' ' << element.uv1.y << ' '
            << ' ';
      }

      out << '\n';
    }
  }
}

void MapEditor::Load(const char *filename) {
  std::ifstream input(filename);
  defer { input.close(); };

  std::string line;
  while (std::getline(input, line)) {
    std::istringstream stream(line);

    char *key = new char[256];
    int element_count;

    if (!(stream >> key >> element_count)) {
      assert(0 && "Failed to parse key and element_count");
    }

    auto hash_map = shget(elements, key);

    for (int i = 0; i < element_count; i++) {
      IVec2 position;
      Element element;
      if (!(stream >> position.x >> position.y
          >> element.uv0.x >> element.uv0.y
          >> element.uv1.x >> element.uv1.y)) {
        assert(0 && "Failed to parse elements");
      }

      hmput(hash_map, position, element);
    }

    shput(elements, key, hash_map);
  }
}

void MapEditor::Undo() {
  if (arrlen(keys)) {
    const char *last_key = arrpop(keys);
    if (last_key != NULL) {
      auto hash_map = shget(elements, last_key);
      auto length = hmlenu(hash_map);
      auto key = hash_map[length - 1].key;
      hmdel(hash_map, key);
    }
  }
}

void MapEditor::Remove(const IVec2 &position) {
  for (int i = 0; i < shlen(elements); i++) {
    auto hash_map = elements[i].value;
    if (hmgeti(hash_map, position) >= 0) {
      hmdel(hash_map, position);
      return;
    }
  }
}