#pragma once

#include <vector>
#include <entt/entt.hpp>

#include "Reflection/Property.h"

struct IComponentReflection
{
    virtual const std::vector<IProperty*>&GetProperties() const = 0;
    virtual bool IsOnEntity(entt::registry& r, entt::entity e) const = 0;
    virtual void* GetOwner(entt::registry& r, entt::entity e) const = 0;
};

template<typename T>
struct ComponentReflection : IComponentReflection
{
    ComponentReflection(std::vector<IProperty*> properties)
        : Properties(properties) {
    }

    std::vector<IProperty*> Properties;

    const std::vector<IProperty*>& GetProperties() const override
    {
        return Properties;
    }

    bool IsOnEntity(entt::registry& r, entt::entity e) const override
    {
        return r.all_of<T>(e);
    }

    void* GetOwner(entt::registry& r, entt::entity e) const override
    {
        if (!IsOnEntity(r, e))
            return nullptr;

        return &r.get<T>(e);
    }
};