#pragma once

#include <glm/glm.hpp>

#include "Reflection/Reflection.h"
#include "Colour.h"

struct Light
{
	INIT_REFLECTION(Light)

	Colour Colour = Colour::White;
	PROPERTY(Colour, PropertyType::Colour)

	REFLECT(Light)
};