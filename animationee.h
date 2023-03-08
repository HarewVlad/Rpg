enum Action_ {
  Action_MoveUp,
  Action_MoveDown,
  Action_MoveLeft,
  Action_MoveRight,
  Action_Attack,
  Action_Roll,
  Action_Die,
  Action_Idle
};

struct Animation {
  const char **keys = NULL;
};

struct Animationee {
  Animation *animations = NULL;
};

// 1. Load animation frames from drive
// 2. Set parameters for animation - tick, action id