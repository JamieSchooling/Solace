#include "Inspectors/LightInspector.h"

void LightInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
	auto typeProp = m_component->GetProperty("Type");
	EnumInfo typeInfo = std::any_cast<EnumInfo>(typeProp->Get(registry, entity));
	if (EditorProperty<EnumInfo>("Type", typeInfo).Draw())
	{
		typeProp->Set(typeInfo.CurrentValue, registry, entity);
	}

	auto colourProp = m_component->GetProperty("Colour");
	Colour colour = std::any_cast<Colour>(colourProp->Get(registry, entity));
	if (EditorProperty<Colour>("Colour", colour).Draw())
	{
		colourProp->Set(colour, registry, entity);
	}

	auto intensityProp = m_component->GetProperty("Intensity");
	float intensity = std::any_cast<float>(intensityProp->Get(registry, entity));
	if (EditorProperty<float>("Intensity", intensity).Draw())
	{
		intensityProp->Set(intensity, registry, entity);
	}

	auto lightType = magic_enum::enum_cast<LightType>(typeInfo.CurrentValue);
	if (lightType.has_value())
	{
		if (lightType.value() == LightType::Point)
		{
			auto radiusProp = m_component->GetProperty("Radius");
			float radius = std::any_cast<float>(radiusProp->Get(registry, entity));
			if (EditorProperty<float>("Radius", radius).Draw())
			{
				radiusProp->Set(radius, registry, entity);
			}
		}
	}
}
