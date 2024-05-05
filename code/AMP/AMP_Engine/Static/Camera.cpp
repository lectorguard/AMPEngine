#include "Camera.h"

glm::vec3 amp::Camera::forward = glm::vec3(0, 0, -1);
glm::vec3 amp::Camera::up = glm::vec3(0, 1, 0);
glm::vec3 amp::Camera::location = glm::vec3(0, 0, 0);
glm::mat4 amp::Camera::projection = glm::mat4(1.0f);