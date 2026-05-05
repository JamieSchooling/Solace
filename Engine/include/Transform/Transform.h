#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Reflection/ComponentReflection.h"
#include "Reflection/Property.h"
#include "Reflection/ReflectionRegistry.h"

struct Transform
{
	glm::vec3 Position{ glm::vec3(0.f) }; //!< Translation i.e. position
	glm::quat Rotation{ glm::quat(glm::vec3(0.f)) }; //!< Orientation as a quaternion
	glm::vec3 Scale{ glm::vec3(1.f) }; //!< Scale

	static inline Property<Transform, &Transform::Position> fovProp{ "Position", PropertyType::Vec3 };
	static inline Property<Transform, &Transform::Rotation> nearProp{ "Rotation", PropertyType::Quaternion };
	static inline Property<Transform, &Transform::Scale> farProp{ "Scale", PropertyType::Vec3 };
	static inline std::vector<IProperty*> props = { &fovProp, &nearProp, &farProp };

	static inline bool transformRegistration = []()
	{
		ReflectionRegistry::Get().push_back(std::make_shared<ComponentReflection<Transform>>("Transform", props));
		return true;
	}();

	glm::vec3 Right() const
	{
		glm::mat4 mat = GetTransformMatrix();
		return glm::normalize(-glm::vec3(mat[0]));
	}

	glm::vec3 Up() const
	{
		glm::mat4 mat = GetTransformMatrix();
		return glm::normalize(-glm::vec3(mat[1]));
	}

	glm::vec3 Forward() const
	{
		glm::mat4 mat = GetTransformMatrix();
		return glm::normalize(-glm::vec3(mat[2]));
	}

	glm::vec3 EulerAngles()
	{
		return glm::eulerAngles(Rotation);
	}

	glm::mat4 GetTransformMatrix() const
	{
		glm::mat4 T = glm::translate(glm::mat4(1.f), Position);
		glm::mat4 R = glm::toMat4(Rotation);
		glm::mat4 S = glm::scale(glm::mat4(1.f), Scale);

		return T * R * S;
	}
};