#include "Inspectors/LightInspector.h"
#include "Inspectors/EditorProperty.h"

void LightInspector::Initialise(entt::registry& r, entt::entity e)
{
	m_typeProp = m_component->GetProperty("Type");
	m_colourProp = m_component->GetProperty("Colour");
	m_intensityProp = m_component->GetProperty("Intensity");
	m_radiusProp = m_component->GetProperty("Radius");
}

void LightInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
	EnumInfo typeInfo = std::any_cast<EnumInfo>(m_typeProp->Get(registry, entity));
	EditResult result = EditorProperty<EnumInfo>("Type", typeInfo).Draw();
	if (result.Changed)
	{
		m_typeProp->Set(typeInfo.CurrentValue, registry, entity);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_typeProp, registry, entity);
	}
	if (result.EditEnded)
	{
		UndoSystem::EndPropertyEdit(registry, entity);
	}

	Colour colour = std::any_cast<Colour>(m_colourProp->Get(registry, entity));
	result = EditorProperty<Colour>("Colour", colour).Draw();
	if (result.Changed)
	{
		m_colourProp->Set(colour, registry, entity);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_colourProp, registry, entity);
	}
	if (result.EditEnded)
	{
		UndoSystem::EndPropertyEdit(registry, entity);
	}

	float intensity = std::any_cast<float>(m_intensityProp->Get(registry, entity));
	result = EditorProperty<float>("Intensity", intensity, m_intensityProp->Attributes()).Draw();
	if (result.Changed)
	{
		m_intensityProp->Set(intensity, registry, entity);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_intensityProp, registry, entity);
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
			float radius = std::any_cast<float>(m_radiusProp->Get(registry, entity));
			result = EditorProperty<float>("Radius", radius, m_radiusProp->Attributes()).Draw();
			if (result.Changed)
			{
				m_radiusProp->Set(radius, registry, entity);
			}
			if (result.EditStarted)
			{
				UndoSystem::BeginPropertyEdit(m_radiusProp, registry, entity);
			}
			if (result.EditEnded)
			{
				UndoSystem::EndPropertyEdit(registry, entity);
			}
		}
	}
}
