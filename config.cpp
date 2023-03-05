// #pragma warning(disable:4244) // Disable stupid "possible loss of data" warning here
// template <typename T>
// void Config::Put(const char *key, T value) {
//   auto type_index = std::type_index(typeid(T));
//   if (type_index == std::type_index(typeid(int))) {
//     shput(integers, key, value);
//   } else if (type_index == std::type_index(typeid(float))) {
//     shput(floats, key, value);
//   } else if (type_index == std::type_index(typeid(char *))) {
//     shput(strings, key, value);
//   } else {
//     assert(0 && "Type is not defined");
//   }
//   #pragma warning(default:4244)
// }

// #pragma warning(disable:4244)
// template <typename T>
// T Config::Get(const char *key) {
//   auto type_index = std::type_index(typeid(T));
//   if (type_index == std::type_index(typeid(int))) {
//     return shget(integers, key);
//   } else if (type_index == std::type_index(typeid(float))) {
//     return shget(floats, key);
//   } else {
//     assert(0 && "Type is not defined");
//     return NULL;
//   }
//   #pragma warning(default:4244)
// }

void Config::Uninitialize() {
  shfree(integers);
  shfree(floats);
}

void Config::Put(const char *key, int value) {
  shput(integers, key, value);
}

void Config::Put(const char *key, float value) {
  shput(floats, key, value);
}

void Config::Put(const char *key, char *value) {
  shput(strings, key, value);
}

int Config::GetInt(const char *key) {
  return shget(integers, key);
}

float Config::GetFloat(const char *key) {
  return shget(floats, key);
}

char *Config::GetString(const char *key) {
  return shget(strings, key);
}