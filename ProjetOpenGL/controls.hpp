#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
void computeMatricesFromInputs(GLFWwindow* window);

glm::mat4 getProjectionMatrix();

glm::mat4 getViewMatrix();

