#include "light.h"

Light::Light(glm::vec3 pos, glm::vec3 col, float power)
{
	vec3Position = pos;
	vec3Color = col;
	fPower = power;
}
