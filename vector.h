// Inside imconfig.h enables casts to ImVec2
struct IVec2 {
  int x;
  int y;

  constexpr IVec2() : x(0), y(0) {}
  constexpr IVec2(int x, int y) : x(x), y(y) {}
};