#pragma once

#include <vector>

#include "Reflection/ComponentReflection.h"

struct ReflectionModule
{
	std::vector<std::shared_ptr<IComponentReflection>> Components;
};

inline ReflectionModule* GetReflectionModule()
{
	static ReflectionModule module;
	return &module;
}