#pragma once

#include <any> // <-- Not great, find a better alternative

struct IComponentReflection;

struct IProperty
{
    virtual ~IProperty() = default;
    virtual const char* Name() const = 0;
    virtual std::any Get(entt::registry& r, entt::entity e) const = 0;
    virtual void Set(const std::any& value, entt::registry& r, entt::entity e) const = 0;
};

template<typename Target, auto Member>
struct Property : public IProperty
{
    Property(const char* n) : name(n) {}

    const char* name;
    using ValueType = std::remove_reference_t<
        decltype(std::declval<Target>().*Member)
    >;

    const char* Name() const override
    {
        return name;
    }

    std::any Get(entt::registry& r, entt::entity e) const override
    {
		Target& o = r.get<Target>(e);
		return o.*Member;
    }

    void Set(const std::any& value, entt::registry& r, entt::entity e) const override
    {
		Target& o = r.get<Target>(e);
		o.*Member = std::any_cast<ValueType>(value);
    }
};