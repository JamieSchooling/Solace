#pragma once

#include "ComponentInspector.h"

#include <Transform/Transform.h>

#include <iostream>

class TransformInspector : public ComponentInspector
{
	using ComponentInspector::ComponentInspector;
protected:
	void Initialise(entt::registry& r, entt::entity e) override;
	void DrawInspector(entt::registry& r, entt::entity e) override;
	void DrawGizmos(entt::registry& registry, entt::entity entity) override;
	void EndFrame(entt::registry& registry, entt::entity entity) override;
private:
	bool m_eulerCached = false;
	Transform* m_transform;
	glm::vec3 m_currentEuler;
	glm::vec3 m_eulerCache;
};
CUSTOM_INSPECTOR(Transform, TransformInspector)