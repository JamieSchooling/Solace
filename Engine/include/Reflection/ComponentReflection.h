#pragma once

#include <entt/entt.hpp>

#include <vector>

#include "Reflection/Property.h"
#include "Reflection/TypeID.h"

struct IComponentReflection
{
	virtual ~IComponentReflection() = default;
	virtual const char* Name() const = 0;
    virtual IProperty* GetProperty(const char* name) const = 0;
    virtual const std::vector<IProperty*>& GetProperties() const = 0;
    virtual bool IsOnEntity(entt::registry& r, entt::entity e) const = 0;
	virtual TypeID GetTypeID() = 0;

    template<typename T>
    T* GetTarget(entt::registry& r, entt::entity e) const 
    {
        if (!IsOnEntity(r, e))
            return nullptr;

        return &r.get<T>(e);
    }

	virtual void* GetTarget(entt::registry& r, entt::entity e) const = 0;
};

template<typename T>
struct ComponentReflection : IComponentReflection
{
    ComponentReflection(const char* name, std::vector<IProperty*> properties)
        : name(name), typeID(Hash(name)), Properties(properties) 
    {}

	const char* name;
	TypeID typeID;

	const char* Name() const override
	{
		return name;
	}

	IProperty* GetProperty(const char* name) const override
	{
		auto it = std::find_if(Properties.begin(), Properties.end(), [&](IProperty* p) { return std::string(p->Name()) == std::string(name); });
		return it != Properties.end() ? *it : nullptr;
	}

    const std::vector<IProperty*>& GetProperties() const override
    {
        return Properties;
    }

    bool IsOnEntity(entt::registry& r, entt::entity e) const override
    {
        return r.all_of<T>(e);
    }

	TypeID GetTypeID()
	{
		return typeID;
	}

	void* GetTarget(entt::registry& r, entt::entity e) const override
	{
		if (!IsOnEntity(r, e))
			return nullptr;

		return &r.get<T>(e);
	}
private:
    std::vector<IProperty*> Properties;
};