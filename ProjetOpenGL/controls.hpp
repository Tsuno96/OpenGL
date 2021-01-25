#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#ifndef CONTROLS_HPP
#define CONTROLS_HPP

float computeMatricesFromInputs(GLFWwindow* window);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif
