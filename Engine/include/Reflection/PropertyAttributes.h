#pragma once

#include <memory>

struct PropertyAttribute 
{
	virtual ~PropertyAttribute() {}
};

struct RangeAttribute : public PropertyAttribute
{
	RangeAttribute(float min, float max) : Min(min), Max(max) {}

	float Min;
	float Max;
};

inline std::shared_ptr<RangeAttribute> Range(float min, float max)
{
	return std::make_shared<RangeAttribute>(min, max);
}

struct MinAttribute : public PropertyAttribute
{
	MinAttribute(float min) : MinValue(min) {}

	float MinValue;
};

inline std::shared_ptr<MinAttribute> Min(float min)
{
	return std::make_shared<MinAttribute>(min);
}

struct HideInInspectorAttribute : public PropertyAttribute
{
};

inline std::shared_ptr<HideInInspectorAttribute> HideInInspector()
{
	return std::make_shared<HideInInspectorAttribute>();
}
