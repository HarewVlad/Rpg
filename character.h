struct Character {
  HashMap<int, int> *animations = NULL; // HashMap<Action, Index>

  void SetAnimation(const ActiveAnimation *active_animation);
};