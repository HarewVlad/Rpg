void MapEditor::AddElement(const char *filename, const ImVec2 &uv0, const ImVec2 &uv1, const IVec2 &position) {
  auto hash_map = shget(elements, filename);
  auto array = hmget(hash_map, position);

  arrput(array, (Element {uv0, uv1}));
  hmput(hash_map, position, array);

  shput(elements, filename, hash_map);
  arrput(filenames, filename);
}

void MapEditor::AddCharacter(const IVec2 &position) {
  hmput(characters, position, (Character {}));
}

void MapEditor::Clear() {
  for (int i = 0; i < shlen(elements); i++) {
    auto hash_map = elements[i].value;
    hmfree(hash_map);
  }
  hmfree(elements);
  arrfree(filenames);
}

int MapEditor::GetGridStep() {
  return grid_steps[grid_step_index];
}

void MapEditor::Save(const char *filename) {
  FILE *file = fopen(filename, "wb+");
  defer { fclose(file); };

  if (file != NULL) {
    char *stream = NULL;
    defer { arrfree(stream); };

    Stream::Append(&stream, &size, sizeof(IVec2));
    
    for (int i = 0; i < shlen(elements); i++) {
      const auto key = elements[i].key;
      const auto hash_map = elements[i].value;
      const auto hash_map_length = hmlen(hash_map);

      Stream::Append(&stream, key, MAX_PATH);
      Stream::Append(&stream, &hash_map_length, sizeof(hash_map_length));

      for (int j = 0; j < hash_map_length; j++) {
        const auto position = hash_map[j].key;
        const auto array = hash_map[j].value;
        const auto array_length = arrlen(array);

        Stream::Append(&stream, &position, sizeof(position));
        Stream::Append(&stream, &array_length, sizeof(array_length));
        Stream::Append(&stream, &array[0], array_length * sizeof(array[0]));
      }
    }
    const auto stream_length = arrlen(stream);

    fwrite(&stream_length, sizeof(stream_length), 1, file);
    fwrite(stream, stream_length, 1, file);
  }
}

void MapEditor::Load(const char *filename) {
  FILE *file = fopen(filename, "rb+");
  defer { fclose(file); };

  if (file != NULL) {
    size_t stream_length;
    fread(&stream_length, sizeof(stream_length), 1, file);

    char *stream = NULL;
    arrsetlen(stream, stream_length);
    
    char *p = stream; // Stream::Read moves "stream" pointer, so need a backup
    defer { arrfree(p); };

    fread(stream, stream_length, 1, file);

    char *key = NULL;
    arrsetlen(key, MAX_PATH);
    memset(key, 0, arrlen(key));

    size_t element_count;

    Stream::Read(&stream, &size, sizeof(size));
    Stream::Read(&stream, key, arrlen(key));
    Stream::Read(&stream, &element_count, sizeof(element_count));

    auto hash_map = shget(elements, key);

    for (int i = 0; i < element_count; i++) {
      IVec2 position;
      size_t array_length;

      Stream::Read(&stream, &position, sizeof(position));
      Stream::Read(&stream, &array_length, sizeof(array_length));

      auto array = hmget(hash_map, position);
      arrsetlen(array, array_length);

      Stream::Read(&stream, &array[0], array_length * sizeof(*array));

      hmput(hash_map, position, array);
    }

    shput(elements, key, hash_map);
  }
}

void MapEditor::Undo() {
  if (arrlen(filenames)) {
    const auto filename = arrpop(filenames);
    if (filename != NULL) {
      auto hash_map = shget(elements, filename);
      const auto length = hmlenu(hash_map);
      const auto key = hash_map[length - 1].key;
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