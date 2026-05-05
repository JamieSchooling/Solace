#pragma once
#include <string>
#include <memory>
#include <vector>
#include "EditorProperty.h"

#include <Reflection/ComponentReflection.h>

class ComponentInspector
{
public:
	ComponentInspector(std::shared_ptr<IComponentReflection> component);

	void Draw(entt::registry& r, entt::entity e);

protected:
	std::shared_ptr<IComponentReflection> m_Component;

	virtual void DrawInspector(entt::registry& r, entt::entity e);
};