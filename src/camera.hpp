#pragma once
#include <glm/glm.hpp>
#include "control.hpp"

class Camera {
public:
  glm::vec3 position = glm::vec3(-15.0f, 50.0f, 50.0f);
  glm::vec3 forward  = glm::vec3(1.0f, 0.0f, 0.0f);
  float speed = 30.0;

  Camera();
  void update(Control& controls, float delta);
};