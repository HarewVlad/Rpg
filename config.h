struct Config {
  HashMap<char *, int> *integers = NULL;
  HashMap<char *, float> *floats = NULL;
  HashMap<char *, char *> *strings = NULL;

  void Put(const char *key, int value);
  void Put(const char *key, float value);
  void Put(const char *key, char *value);

  int GetInt(const char *key);
  float GetFloat(const char *key);
  char *GetString(const char *key);

  void Uninitialize();
};