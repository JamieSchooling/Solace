#include "Reflection/PropertyAttributes.h"

std::shared_ptr<RangeAttribute> Range(float min, float max)
{
	return std::make_shared<RangeAttribute>(min, max);
}

std::shared_ptr<MinAttribute> Min(float min)
{
	return std::make_shared<MinAttribute>(min);
}
