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

std::shared_ptr<RangeAttribute> Range(float min, float max);

struct MinAttribute : public PropertyAttribute
{
	MinAttribute(float min) : MinValue(min) {}

	float MinValue;
};

std::shared_ptr<MinAttribute> Min(float min);