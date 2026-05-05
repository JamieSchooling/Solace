#pragma once

#include <memory>
#include <Reflection/TypeID.h>
#include <Reflection/ComponentReflection.h>
#include <unordered_map>

#include "ComponentInspector.h"

struct InspectorRegistry
{
	using Factory = std::shared_ptr<ComponentInspector>(*)(std::shared_ptr<IComponentReflection>);
	static std::unordered_map<TypeID, Factory>& Get()
	{
		static std::unordered_map<TypeID, Factory> registry;
		return registry;
	}
};

template<typename Inspector, typename = std::enable_if_t<std::is_base_of_v<ComponentInspector, Inspector>>>
std::shared_ptr<ComponentInspector> CustomInspector(std::shared_ptr<IComponentReflection> reflection)
{
	return std::make_shared<Inspector>(reflection);
}