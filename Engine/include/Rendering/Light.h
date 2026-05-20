#pragma once

#include <glm/glm.hpp>

#include "Reflection/Reflection.h"
#include "Colour.h"

enum class LightType
{
	Directional,
	Point
};

struct Light
{
	INIT_REFLECTION(Light)

	LightType Type = LightType::Directional;
	PROPERTY(Type)

	Colour Colour = Colour::White;
	PROPERTY(Colour)

	float Intensity = 1.0f;
	PROPERTY(Intensity)

	float Radius = 5.0f;
	PROPERTY(Radius)

	REFLECT(Light)
};