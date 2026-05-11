#pragma once

#include "Reflection/ComponentReflection.h"
#include <iostream>
#include <cstdint>

#include <Scenes/NameComponent.h>

struct ReflectionRegistry
{
    static std::vector<std::shared_ptr<IComponentReflection>>& Get()
    {
		static std::vector<std::shared_ptr<IComponentReflection>> components;
        return components;
    }

    static auto View(entt::registry& r, entt::entity e)
    {
		return Get() | std::views::filter([e, &r](std::shared_ptr<IComponentReflection> c)
		{
			return c->IsOnEntity(r, e);
		});
    }

    static std::shared_ptr<IComponentReflection> Get(const char* name)
    {
		auto it = std::find_if(Get().begin(), Get().end(), [&](std::shared_ptr<IComponentReflection> c) { return std::string(c->Name()) == std::string(name); });
		return it != Get().end() ? *it : nullptr;
    }
};