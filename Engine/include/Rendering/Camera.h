#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Rendering/Window.h"

#include "Reflection/Reflection.h"

enum class CameraProjectionType
{
	Perspective,
	Orthographic
};

struct Camera
{
public:
	Camera();

	INIT_REFLECTION(Camera)

	float FOV = 70.0f;
	PROPERTY(FOV, PropertyType::Float)

	float Near = 0.1f;
	PROPERTY(Near, PropertyType::Float)

	float Far = 100.0f;
	PROPERTY(Far, PropertyType::Float)

	CameraProjectionType ProjectionType = CameraProjectionType::Perspective;
	
	REFLECT(Camera)
private:
	glm::mat4 m_Projection = glm::mat4(1.0f);

public:
	void RecalculateProjection();
	const glm::mat4& GetProjection() const;
};	