namespace Test {
  static void TestConfig() {
    Config config;
    defer { config.Uninitialize(); };

    config.Put("Key", 1);
    config.Put("Key", 1.111f);
    assert(config.GetInt("Key") == 1);
    assert(config.GetFloat("Key") == 1.111f);
  }
};