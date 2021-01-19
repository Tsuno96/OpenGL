#include "controls.hpp"
#include <glm\gtc\matrix_transform.hpp>

glm::mat4 ProjectionMatrix;
glm::mat4 ViewMatrix;

//position
glm::vec3 position = glm::vec3(0, 0, 5);
//horizontal angle : toward -Z
float horizontalAngle = 3.14f;
//vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
//Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

void computeMatricesFromInputs(GLFWwindow* window)
{
	static double lastTime = glfwGetTime();

	//compute deltaTime
	float currentTime = glfwGetTime();

	//currentTime - lastTime
	float deltaTime = currentTime - lastTime;

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	//Get size window
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, width / 2, height / 2);

	//compute new orientation
	horizontalAngle += mouseSpeed * float(width / 2 - xpos);
	verticalAngle += mouseSpeed * float(height / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector : perpendicular to both direction and right
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;

	//glm::mat4 Projection = glm::ortho(-(float)width * multiplier, (float)width * multiplier, (float)height * multiplier, -(float)height * multiplier, -1.f, 10.f);
	// Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

	float multiplier = 0.01f;
	//ProjectionMatrix = glm::ortho(-(float)width * multiplier, (float)width * multiplier, -(float)height * multiplier, (float)height * multiplier, -1.f, 10.f);
	ProjectionMatrix = glm::perspective(FoV, float(width) / float(height), 0.1f, 100.0f);

	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	lastTime = currentTime;
}

glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}