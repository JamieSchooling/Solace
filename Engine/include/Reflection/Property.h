#pragma once

#include <any> // <-- Not great, find a better alternative
#include <entt/entt.hpp>

struct IComponentReflection;

enum class PropertyType
{
	Bool,
	Int,
	Float,
	Vec2,
	Vec3,
	Vec4,
	Quaternion,
	String
};

struct IProperty
{
    virtual ~IProperty() = default;
    virtual const char* Name() const = 0;
    virtual PropertyType Type() const = 0;
    virtual std::any Get(entt::registry& r, entt::entity e) const = 0;
    virtual void Set(const std::any& value, entt::registry& r, entt::entity e) const = 0;
};

template<typename Target, auto Member>
struct Property : public IProperty
{
    Property(const char* n, PropertyType type) : m_name(n), m_type(type) {}

    using ValueType = std::remove_reference_t<
        decltype(std::declval<Target>().*Member)
    >;

	PropertyType Type() const override
	{
		return m_type;
	}

    const char* Name() const override
    {
        return m_name;
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
private:
	const char* m_name;
	const PropertyType m_type;
};