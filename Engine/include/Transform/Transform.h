#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct Transform
{
	glm::vec3 Position{ glm::vec3(0.f) }; //!< Translation i.e. position
	glm::quat Rotation{ glm::quat(glm::vec3(0.f)) }; //!< Orientation as a quaternion
	glm::vec3 Scale{ glm::vec3(1.f) }; //!< Scale

	glm::mat4 GetTransformMatrix()
	{
		glm::mat4 T = glm::translate(glm::mat4(1.f), Position);
		glm::mat4 R = glm::toMat4(Rotation);
		glm::mat4 S = glm::scale(glm::mat4(1.f), Scale);

		return T * R * S;
	}
};