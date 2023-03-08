namespace Test {
  static void TestConfig() {
    Config config;
    defer { config.Uninitialize(); };

    config.Put<int>("A", 1);
    config.Put<float>("B", 1.111f);
    assert(config.Get<int>("A") == 1);
    assert(config.Get<float>("B") == 1.111f);
  }
};