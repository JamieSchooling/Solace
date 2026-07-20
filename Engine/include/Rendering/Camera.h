#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Rendering/Window.h"

#include "Reflection/Reflection.h"
#include <Transform/Transform.h>

enum class CameraProjectionType
{
	Perspective,
	Orthographic
};

struct Camera
{
public:
	Camera();

	INIT_REFLECTION(Camera);

	float FOV = 70.0f;
	PROPERTY(FOV, Range(10, 180));

	float Size = 2.0f;
	PROPERTY(Size, Min(0.0f));

	float Near = 0.1f;
	PROPERTY(Near);

	float Far = 100.0f;
	PROPERTY(Far);

	CameraProjectionType ProjectionType = CameraProjectionType::Perspective;
	PROPERTY(ProjectionType);
	
	REFLECT(Camera);

	void Initialise();

	glm::mat4 GetView(Transform& transform);
private:
	glm::mat4 m_projection = glm::mat4(1.0f);

public:
	void RecalculateProjection();
	void RecalculateProjection(glm::vec2 size);
	const glm::mat4& GetProjection() const;
};	