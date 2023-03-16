struct Config {
  char *stream = NULL;
  HashMap<char *, size_t> *indexes = NULL;

  template <typename T>
  void Put(const char *key, const T &data);

  template <typename T>
  const T Get(const char *key);

  void Uninitialize();
};