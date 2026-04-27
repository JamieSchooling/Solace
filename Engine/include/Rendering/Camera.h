#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Rendering/Window.h"

enum class CameraProjectionType
{
	Perspective,
	Orthographic
};

struct Camera
{
public:
	Camera();

	float FOV = 30.0f;
	float Near = 0.1f;
	float Far = 100.0f;
	CameraProjectionType ProjectionType = CameraProjectionType::Perspective;

private:
	glm::mat4 m_Projection = glm::mat4(1.0f);

public:
	void RecalculateProjection();
	const glm::mat4& GetProjection() const;
};	