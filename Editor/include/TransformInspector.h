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
	bool m_eulerCached = false;
	glm::vec3 m_currentEuler;
	glm::vec3 m_eulerCache;
};
CUSTOM_INSPECTOR(Transform, TransformInspector)