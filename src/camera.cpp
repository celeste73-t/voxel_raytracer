#include "camera.hpp"

Camera::Camera() = default;

void Camera::update(Control& controls, float delta) {
  glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
  glm::vec3 movement = glm::vec3(0.0f, 0.0f, 0.0f);

  if (controls.forward)  movement += forward;
  if (controls.backward) movement -= forward;
  if (controls.right)    movement += right;
  if (controls.left)     movement -= right;
  if (controls.up)       movement += glm::vec3(0.0f, 1.0f, 0.0f);
  if (controls.down)     movement -= glm::vec3(0.0f, 1.0f, 0.0f);

  const float movement_length = glm::length(movement);
  if (movement_length > 0.0001f) {  // seuil plutôt que comparaison stricte à 0
    position += (movement / movement_length) * speed * delta;
  }
}