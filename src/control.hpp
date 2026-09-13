#pragma once
#include <SDL2/SDL.h>

class Control
{
public:
  bool forward = false;
  bool backward = false;
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;

  Control();
  bool update();

private:
  void set_key_state(SDL_Keycode keycode, bool pressed);
};