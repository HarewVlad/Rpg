enum Action_ {
  Action_MoveUp = 0,
  Action_MoveDown,
  Action_MoveLeft,
  Action_MoveRight,
  Action_Attack,
  Action_Roll,
  Action_Die,
  Action_Idle
};

static const char* Action_String[] = {
  "Action_MoveUp",
  "Action_MoveDown",
  "Action_MoveLeft",
  "Action_MoveRight",
  "Action_Attack",
  "Action_Roll",
  "Action_Die",
  "Action_Idle"
};

struct Animation {
  const char **filenames = NULL;
  int action = 0;
  int index = 0;
  float time = 0.0f;

  void Update(float dt);
  void Reset();
};

struct ActiveAnimation {
  int index;
  int action;
};

struct Animationee {
  Animation *animations = NULL;
  ActiveAnimation active_animation;
};