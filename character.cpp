void Character::SetAnimation(const ActiveAnimation *active_animation) {
  hmput(animations, active_animation->action, active_animation->index);
}