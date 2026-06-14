#include "Inspectors/LightInspector.h"
#include "Inspectors/EditorProperty.h"

void LightInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
	auto typeProp = m_component->GetProperty("Type");
	EnumInfo typeInfo = std::any_cast<EnumInfo>(typeProp->Get(registry, entity));
	EditResult result = EditorProperty<EnumInfo>("Type", typeInfo).Draw();
	if (result.Changed)
	{
		typeProp->Set(typeInfo.CurrentValue, registry, entity);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(typeProp, registry, entity);
	}
	if (result.EditEnded)
	{
		UndoSystem::EndPropertyEdit(registry, entity);
	}

	auto colourProp = m_component->GetProperty("Colour");
	Colour colour = std::any_cast<Colour>(colourProp->Get(registry, entity));
	result = EditorProperty<Colour>("Colour", colour).Draw();
	if (result.Changed)
	{
		colourProp->Set(colour, registry, entity);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(colourProp, registry, entity);
	}
	if (result.EditEnded)
	{
		UndoSystem::EndPropertyEdit(registry, entity);
	}

	auto intensityProp = m_component->GetProperty("Intensity");
	float intensity = std::any_cast<float>(intensityProp->Get(registry, entity));
	result = EditorProperty<float>("Intensity", intensity).Draw();
	if (result.Changed)
	{
		intensityProp->Set(intensity, registry, entity);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(intensityProp, registry, entity);
	}
	if (result.EditEnded)
	{
		UndoSystem::EndPropertyEdit(registry, entity);
	}

	auto lightType = magic_enum::enum_cast<LightType>(typeInfo.CurrentValue);
	if (lightType.has_value())
	{
		if (lightType.value() == LightType::Point)
		{
			auto radiusProp = m_component->GetProperty("Radius");
			float radius = std::any_cast<float>(radiusProp->Get(registry, entity));
			result = EditorProperty<float>("Radius", radius).Draw();
			if (result.Changed)
			{
				radiusProp->Set(radius, registry, entity);
			}
			if (result.EditStarted)
			{
				UndoSystem::BeginPropertyEdit(radiusProp, registry, entity);
			}
			if (result.EditEnded)
			{
				UndoSystem::EndPropertyEdit(registry, entity);
			}
		}
	}
}
