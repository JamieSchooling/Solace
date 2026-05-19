#pragma once

#include <glm/glm.hpp>

struct Colour
{
	Colour(glm::vec4 colour = White) : ColourValue(colour) {}
	glm::vec4 ColourValue;

	static constexpr glm::vec4 White = glm::vec4(1.0f);
	static constexpr glm::vec4 Black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static constexpr glm::vec4 Red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	static constexpr glm::vec4 Green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	static constexpr glm::vec4 Blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
};