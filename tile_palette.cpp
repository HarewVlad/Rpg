void TilePalette::Clear() {
  hmfree(active_tiles);
}

void TilePalette::SetActive(const char *key) {
  if (active_key != key) {
    active_key = key;
    Clear();
  }
}

void TilePalette::AddTiles() {
  for (int x = start_tile.x; x <= end_tile.x; x++) {
    for (int y = start_tile.y; y <= end_tile.y; y++) {
      IVec2 tile = ImVec2(x, y);
      hmput(active_tiles, tile, 1);
    }
  }
}