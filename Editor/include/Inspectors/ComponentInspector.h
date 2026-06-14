#pragma once
#include <string>
#include <memory>
#include <vector>

#include <Reflection/ComponentReflection.h>
#include "InspectorRegistry.h"

#include <Rendering/Camera.h>

class ComponentInspector
{
public:
	ComponentInspector(std::shared_ptr<IComponentReflection> component);

	virtual void Initialise(entt::registry& r, entt::entity e) {}
	void Draw(entt::registry& r, entt::entity e);
	virtual void DrawGizmos(entt::registry& registry, entt::entity entity) {}
	virtual void EndFrame(entt::registry& registry, entt::entity entity) {}

	std::shared_ptr<IComponentReflection> GetComponent() { return m_component; }
protected:
	std::shared_ptr<IComponentReflection> m_component;

	virtual void DrawInspector(entt::registry& r, entt::entity e); 
};

#define CUSTOM_INSPECTOR(Component, Inspector) \
	static bool _reg_##Inspector = []() \
	{ \
		InspectorRegistry::Get()[Hash(#Component)] = &CustomInspector<Inspector>; \
		return true; \
	}();