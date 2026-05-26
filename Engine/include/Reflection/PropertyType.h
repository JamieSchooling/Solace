#pragma once

#include "Reflection/Property.h"
#include "Rendering/Colour.h"

#include <glm/glm.hpp>
#include <Assets/AssetRegistry.h>

enum class PropertyType
{
	Unknown,
	Bool,
	Int,
	Float,
	Vec2,
	Vec3,
	Vec4,
	Quaternion,
	String,
	Colour,
	Enum,
	Asset,
};

template<typename T>
struct PropertyTypeResolver
{
	static constexpr PropertyType Type =
		std::is_enum_v<T>
		? PropertyType::Enum
		: PropertyType::Unknown;
};

template<>
struct PropertyTypeResolver<bool>
{
	static constexpr PropertyType Type = PropertyType::Bool;
};

template<>
struct PropertyTypeResolver<float>
{
	static constexpr PropertyType Type = PropertyType::Float;
};

template<>
struct PropertyTypeResolver<int>
{
	static constexpr PropertyType Type = PropertyType::Int;
};

template<>
struct PropertyTypeResolver<glm::vec2>
{
	static constexpr PropertyType Type = PropertyType::Vec2;
};

template<>
struct PropertyTypeResolver<glm::vec3>
{
	static constexpr PropertyType Type = PropertyType::Vec3;
};

template<>
struct PropertyTypeResolver<glm::vec4>
{
	static constexpr PropertyType Type = PropertyType::Vec4;
};

template<>
struct PropertyTypeResolver<glm::quat>
{
	static constexpr PropertyType Type = PropertyType::Quaternion;
};

template<>
struct PropertyTypeResolver<std::string>
{
	static constexpr PropertyType Type = PropertyType::String;
};

template<>
struct PropertyTypeResolver<Colour>
{
	static constexpr PropertyType Type = PropertyType::Colour;
};

template<>
struct PropertyTypeResolver<AssetHandle>
{
	static constexpr PropertyType Type = PropertyType::Asset;
};