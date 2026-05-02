#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Rendering/Window.h"

#include "Reflection/ComponentReflection.h"
#include "Reflection/Property.h"
#include "Reflection/ReflectionRegistry.h"

enum class CameraProjectionType
{
	Perspective,
	Orthographic
};

struct Camera
{
public:
	Camera();

	float FOV = 70.0f;
	float Near = 0.1f;
	float Far = 100.0f;
	CameraProjectionType ProjectionType = CameraProjectionType::Perspective;
	
	static inline Property<Camera, &Camera::FOV> fovProp{ "FOV" };
	static inline Property<Camera, &Camera::Near> nearProp{ "Near" };
	static inline Property<Camera, &Camera::Far> farProp{ "Far" };
	static inline std::vector<IProperty*> props = { &fovProp, &nearProp, &farProp };

	static inline bool cameraRegistration = []() {
		ReflectionRegistry::Get().push_back(std::make_shared<ComponentReflection<Camera>>(props));
		return true;
	}();

private:
	glm::mat4 m_Projection = glm::mat4(1.0f);

public:
	void RecalculateProjection();
	const glm::mat4& GetProjection() const;
};	