#pragma once

#include <entt/entt.hpp>

#include <vector>

#include "Reflection/Property.h"
#include "Reflection/TypeID.h"

struct IComponentReflection
{
	virtual ~IComponentReflection() = default;
	virtual const char* Name() const = 0;
    virtual IProperty* GetProperty(const char* m_name) const = 0;
    virtual const std::vector<IProperty*>& GetProperties() const = 0;
    virtual bool IsOnEntity(entt::registry& r, entt::entity e) const = 0;
    virtual void Emplace(entt::registry& r, entt::entity e) const = 0;
    virtual void Initialise(entt::registry& r, entt::entity e) const = 0;
	virtual TypeID GetTypeID() = 0;

    template<typename T>
    T* GetTarget(entt::registry& r, entt::entity e) const 
    {
        if (!IsOnEntity(r, e))
            return nullptr;

        return &r.get<T>(e);
    }
};

template<typename T>
struct ComponentReflection : IComponentReflection
{
    ComponentReflection(const char* m_name, std::vector<IProperty*> properties)
        : m_name(m_name), m_typeID(Hash(m_name)), m_properties(properties) 
    {}

	const char* Name() const override
	{
		return m_name;
	}

	IProperty* GetProperty(const char* m_name) const override
	{
		auto it = std::find_if(m_properties.begin(), m_properties.end(), [&](IProperty* p) { return std::string(p->Name()) == std::string(m_name); });
		return it != m_properties.end() ? *it : nullptr;
	}

    const std::vector<IProperty*>& GetProperties() const override
    {
        return m_properties;
    }

    bool IsOnEntity(entt::registry& r, entt::entity e) const override
    {
        return r.all_of<T>(e);
    }

	void Emplace(entt::registry& r, entt::entity e) const override
	{
		r.emplace<T>(e);
	}

	void Initialise(entt::registry& r, entt::entity e) const override
	{
		T* component = GetTarget<T>(r, e);
		if constexpr (requires { component->Initialise(); })
		{
			component->Initialise();
		}
	}

	TypeID GetTypeID()
	{
		return m_typeID;
	}
private:
	const char* m_name;
	TypeID m_typeID;
    std::vector<IProperty*> m_properties;
};