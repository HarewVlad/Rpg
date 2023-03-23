void Animation::Update(float dt) {
  size_t frames = arrlen(filenames);
  if (time > 1.0f / frames) {
    index = (index + 1) % frames;
    time = 0.0f;
  } else {
    time += dt;
  }
}

void Animation::Reset() {
  time = 0.0f;
  index = 0;
}