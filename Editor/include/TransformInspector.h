#pragma once

#include "ComponentInspector.h"

#include <Transform/Transform.h>

#include <iostream>

class TransformInspector : public ComponentInspector
{
	using ComponentInspector::ComponentInspector;
protected:
	void DrawInspector(entt::registry& r, entt::entity e) override;
private:
	bool m_EulerCached = false;
	glm::vec3 m_CurrentEuler;
	glm::vec3 m_EulerCache;
};
CUSTOM_INSPECTOR(Transform, TransformInspector)