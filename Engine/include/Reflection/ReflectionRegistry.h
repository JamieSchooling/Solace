#pragma once

#include "Reflection/ComponentReflection.h"

struct ReflectionRegistry
{
    static std::vector<std::shared_ptr<IComponentReflection>>& Get()
    {
        return handlers;
    }

    static std::vector<std::shared_ptr<IComponentReflection>> Get(entt::registry& r, entt::entity e)
    {
        std::vector<std::shared_ptr<IComponentReflection>> entityHandlers; // TODO: move elsewhere so it's not recreated every call 
        for (auto& handler : handlers)
        {
            if (handler->IsOnEntity(r, e))
            {
                entityHandlers.push_back(handler);
            }
        }
        return entityHandlers;
    }

private:
    static inline std::vector<std::shared_ptr<IComponentReflection>> handlers;
};