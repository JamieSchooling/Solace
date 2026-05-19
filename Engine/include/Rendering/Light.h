#pragma once

#include <glm/glm.hpp>

#include "Reflection/Reflection.h"

struct Light
{
	INIT_REFLECTION(Light)

	glm::vec3 Colour = glm::vec3(1.0f);
	PROPERTY(Colour, PropertyType::Vec3)

	REFLECT(Light)
};