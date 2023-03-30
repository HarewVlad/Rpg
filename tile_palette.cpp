void TilePalette::Clear() {
  hmfree(active_tiles);
}

void TilePalette::SetActive(const char *key) {
  if (active_key != key) {
    active_key = key;
    Clear();
  }
}