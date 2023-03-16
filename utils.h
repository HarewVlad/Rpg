template <typename T, typename V>
struct HashMap {
  T key;
  V value;
};

template <typename T>
struct Set {
  T key;
};

template <typename F>
struct Defer {
  Defer(F f) : f(f) {}
  ~Defer() { f(); }
  F f;
};

template <typename F>
Defer<F> makeDefer(F f) {
  return Defer<F>(f);
};

#define __defer(line) defer_ ## line
#define _defer(line) __defer(line)

struct defer_dummy { };
template<typename F>
Defer<F> operator+( defer_dummy, F&& f )
{
    return makeDefer<F>(std::forward<F>(f));
}

#define defer auto _defer(__LINE__) = defer_dummy() + [&]()

#define Stringify(x) #x
#define ToString(x) Stringify(x)

namespace Utils {
  static int GetMilliseconds();
  static void FlagAdd(int *flags, int flag);
  static void FlagRemove(int *flags, int flag);
  static bool FlagCheck(int flags, int flag);
  static const char *SelectFile();
  static const char *SaveFile();
  static const char *SelectFiles();
};

namespace Stream {
  static size_t Append(char **stream, const void *data, size_t size);
  static void Insert(char **stream,  size_t index, const void *data, size_t size);

  static void Read(char **stream, void *data, size_t size);
};