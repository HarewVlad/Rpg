void MapEditor::AddElement(const char *key, const ImVec2 &uv0, const ImVec2 &uv1, const IVec2 &position) {
  auto hash_map = shget(elements, key);
  auto array = hmget(hash_map, position);

  arrput(array, (Element {uv0, uv1}));
  hmput(hash_map, position, array);

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

void MapEditor::Save(const char *filename) {
  std::ofstream out(filename, std::ofstream::trunc);
  defer { out.close(); };

  if (out) {
    for (int i = 0; i < shlen(elements); i++) {
      const auto key = elements[i].key;
      const auto hash_map = elements[i].value;

      out << key << ' ' << hmlen(hash_map) << ' ';

      for (int j = 0; j < hmlen(hash_map); j++) {
        const auto position = hash_map[j].key;
        const auto array = hash_map[j].value;

        out << position.x << ' ' << position.y << ' ' << arrlen(array) << ' ';

        for (int k = 0; k < arrlen(array); k++) {
          const auto element = array[k];

          out << element.uv0.x << ' ' << element.uv0.y << ' '
              << element.uv1.x << ' ' << element.uv1.y << ' '
              << ' ';
        }
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

    char *key = NULL;
    arrsetlen(key, 256);
    memset(key, 0, arrlen(key));
    int element_count;

    if (!(stream >> key >> element_count)) {
      assert(0 && "Failed to parse key and element_count");
    }

    auto hash_map = shget(elements, key);

    for (int i = 0; i < element_count; i++) {
      IVec2 position;
      int length;

      if (!(stream >> position.x >> position.y >> length)) {
        assert(0 && "Failed to get position and length");
      }

      auto array = hmget(hash_map, position);

      for (int j = 0; j < length; j++) {
        Element element;

        if (!(stream >> element.uv0.x >> element.uv0.y
                     >> element.uv1.x >> element.uv1.y)) {
          assert(0 && "Failed to parse elements");
        }

        arrput(array, element);
      }

      hmput(hash_map, position, array);
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