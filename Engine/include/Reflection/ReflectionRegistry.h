#pragma once

#include "Reflection/ComponentReflection.h"

struct ReflectionRegistry
{
    static std::vector<std::shared_ptr<IComponentReflection>>& Get()
    {
        return handlers;
    }

    static auto View(entt::registry& r, entt::entity e)
    {
		return handlers | std::views::filter([&](std::shared_ptr<IComponentReflection> c)
		{
			return c->IsOnEntity(r, e);
		});
    }

private:
    static inline std::vector<std::shared_ptr<IComponentReflection>> handlers;
};