#include "control.hpp"

Control::Control() = default;

bool Control::update()
{
  bool quit = false;
  SDL_Event event{};

  while (SDL_PollEvent(&event) != 0)
  {
    if (event.type == SDL_QUIT)
    {
      quit = true;
      continue;
    }

    if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
    {
      continue;
    }

    const bool pressed = event.type == SDL_KEYDOWN;
    const SDL_Keycode keycode = event.key.keysym.sym;

    if (pressed && keycode == SDLK_q)
    {
      quit = true;
    }

    set_key_state(keycode, pressed);
  }

  return quit;
}

void Control::set_key_state(SDL_Keycode keycode, bool pressed)
{
  switch (keycode)
  {
    case SDLK_w:
      forward = pressed;
      break;
    case SDLK_s:
      backward = pressed;
      break;
    case SDLK_a:
      left = pressed;
      break;
    case SDLK_d:
      right = pressed;
      break;
    case SDLK_SPACE:
      up = pressed;
      break;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
      down = pressed;
      break;
    default:
      break;
  }
}