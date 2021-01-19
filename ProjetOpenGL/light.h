#pragma once
#include <glm/glm.hpp>
class Light
{
private:
	glm::vec3 vec3Position;
	glm::vec3 vec3Color;
	float fPower;

public:
	Light(glm::vec3 pos, glm::vec3 col, float power);
	glm::vec3 GetPosition() { return vec3Position; }
	glm::vec3 GetColor() { return vec3Color; }
	float GetPower() { return fPower; }
};


