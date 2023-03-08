void Config::Uninitialize() {
  arrfree(stream);
  shfree(indexes);
}

template <typename T>
void Config::Put(const char *key, const T &data) {
  auto index = arraddnindex(stream, sizeof(T));
  memcpy((void *)&stream[index], &data, sizeof(T));
  shput(indexes, key, index);
}

template <typename T>
const T Config::Get(const char *key) {
  auto index = shget(indexes, key);
  return *reinterpret_cast<const T *>(&stream[index]);
}