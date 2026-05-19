#pragma once

#include <any> // <-- Not great, find a better alternative
#include <entt/entt.hpp>
#include "Reflection/PropertyType.h"

#include <magic_enum/magic_enum.hpp>

struct IComponentReflection;

struct EnumEntry
{
	int Value;
	std::string Name;
};

struct EnumInfo
{
	int CurrentValue = 0;

	std::vector<EnumEntry> Entries;

	std::function<void(int)> SetValue;
};

struct IProperty
{
    virtual ~IProperty() = default;
    virtual const char* Name() const = 0;
    virtual PropertyType Type() const = 0;
    virtual std::any Get(entt::registry& r, entt::entity e) const = 0;
    virtual void Set(const std::any& value, entt::registry& r, entt::entity e) const = 0; 
	virtual EnumInfo GetEnumInfo(entt::registry& r, entt::entity e) const = 0;
};

template<typename Target, auto Member>
struct Property : public IProperty
{
    Property(const char* n) : m_name(n), m_type(PropertyTypeResolver<ValueType>::Type) {}

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
		if constexpr (std::is_enum_v<ValueType>)
		{
			return GetEnumInfo(r, e);
		}
		else
		{
			return o.*Member;
		}
    }

    void Set(const std::any& value, entt::registry& r, entt::entity e) const override
    {
		Target& o = r.get<Target>(e);
		if constexpr (std::is_enum_v<ValueType>)
		{
			EnumInfo enumInfo = std::any_cast<EnumInfo>(value);
			enumInfo.SetValue(enumInfo.CurrentValue);
		}
		else
		{
			o.*Member = std::any_cast<ValueType>(value);
		}
    }

	EnumInfo GetEnumInfo(entt::registry& r, entt::entity e) const override
	{
		EnumInfo result;
		if constexpr (std::is_enum_v<ValueType>)
		{
			Target& o = r.get<Target>(e);
			ValueType& value = o.*Member;
			result.CurrentValue = static_cast<int>(value);

			for (auto enumValue : magic_enum::enum_values<ValueType>())
			{
				result.Entries.push_back({
					static_cast<int>(enumValue),
					std::string(magic_enum::enum_name(enumValue))
				});
			}

			result.SetValue = [&value](int newValue)
			{
				value = static_cast<ValueType>(newValue);
			};

		}
		return result;
	}
private:
	const char* m_name;
	const PropertyType m_type;
};