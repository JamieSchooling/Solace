#pragma once

#include "ComponentInspector.h"

#include <Transform/Transform.h>

class TransformInspector : public ComponentInspector
{
public:
	TransformInspector(Transform& transform);
private:
	Transform& m_Transform;
	glm::vec3 m_CurrentEuler;
	glm::vec3 m_EulerCache;

	void OnRotationEdit(glm::vec3& euler);
};