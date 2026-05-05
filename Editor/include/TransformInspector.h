#pragma once

#include "ComponentInspector.h"
#include "InspectorRegistry.h"

#include <Transform/Transform.h>

#include <iostream>
#include <Reflection/TypeID.h>


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
static bool tInspectorReg = []()
{
	InspectorRegistry::Get()[Hash("Transform")] = &CustomInspector<TransformInspector>;
	return true;
}();