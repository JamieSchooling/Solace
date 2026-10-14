#include "Inspectors/CameraInspector.h"
#include "Inspectors/EditorProperty.h"

void CameraInspector::Initialise(entt::registry& r, entt::entity e)
{
	m_projTypeProp = m_component->GetProperty("ProjectionType");
	m_fovProp = m_component->GetProperty("FOV");
	m_sizeProp = m_component->GetProperty("Size");
	m_nearProp = m_component->GetProperty("Near");
	m_farProp = m_component->GetProperty("Far");
	m_cascadeCountProp = m_component->GetProperty("ShadowCascadeCount");
	m_cascadeDistributionProp = m_component->GetProperty("ShadowCascadeDistribution");
}

void CameraInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
	EnumInfo projectionInfo = std::any_cast<EnumInfo>(m_projTypeProp->Get(registry, entity));
	EditResult result = EditorProperty<EnumInfo>("Projection Type", projectionInfo).Draw();
	if (result.Changed)
	{
		m_projTypeProp->Set(projectionInfo.CurrentValue, registry, entity);
		m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_projTypeProp, registry, entity);
	}
	if (result.EditEnded)
	{
		UndoSystem::EndPropertyEdit(registry, entity, [this, entity, &registry](bool isUndo) { m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection(); });
	}

	auto projectionType = magic_enum::enum_cast<CameraProjectionType>(projectionInfo.CurrentValue);
	if (projectionType.has_value())
	{
		if (projectionType.value() == CameraProjectionType::Perspective)
		{
			float fov = std::any_cast<float>(m_fovProp->Get(registry, entity));
			EditResult result = EditorProperty<float>("FOV", fov, m_fovProp->Attributes()).Draw();
			if (result.Changed)
			{
				m_fovProp->Set(fov, registry, entity);
				m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
			}
			if (result.EditStarted)
			{
				UndoSystem::BeginPropertyEdit(m_fovProp, registry, entity);
			}
			if (result.EditEnded)
			{
				auto component = m_component;
				UndoSystem::EndPropertyEdit(registry, entity, [component, entity, &registry](bool isUndo) { component->GetTarget<Camera>(registry, entity)->RecalculateProjection(); });
			}
		}
		else if (projectionType.value() == CameraProjectionType::Orthographic)
		{
			float size = std::any_cast<float>(m_sizeProp->Get(registry, entity));
			EditResult result = EditorProperty<float>("Size", size, m_sizeProp->Attributes()).Draw();
			if (result.Changed)
			{
				m_sizeProp->Set(size, registry, entity);
				m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
			}
			if (result.EditStarted)
			{
				UndoSystem::BeginPropertyEdit(m_sizeProp, registry, entity);
			}
			if (result.EditEnded)
			{
				auto component = m_component;
				UndoSystem::EndPropertyEdit(registry, entity, [component, entity, &registry](bool isUndo) { component->GetTarget<Camera>(registry, entity)->RecalculateProjection(); });
			}
		}
	}

	float near = std::any_cast<float>(m_nearProp->Get(registry, entity));
	result = EditorProperty<float>("Near", near).Draw();
	if (result.Changed)
	{
		m_nearProp->Set(near, registry, entity);
		m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_nearProp, registry, entity);
	}
	if (result.EditEnded)
	{
		auto component = m_component;
		UndoSystem::EndPropertyEdit(registry, entity, [component, entity, &registry](bool isUndo) { component->GetTarget<Camera>(registry, entity)->RecalculateProjection(); });
	}

	float far = std::any_cast<float>(m_farProp->Get(registry, entity));
	result = EditorProperty<float>("Far", far).Draw();
	if (result.Changed)
	{
		m_farProp->Set(far, registry, entity);
		m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_farProp, registry, entity);
	}
	if (result.EditEnded)
	{
		auto component = m_component;
		UndoSystem::EndPropertyEdit(registry, entity, [component, entity, &registry](bool isUndo) { component->GetTarget<Camera>(registry, entity)->RecalculateProjection(); });
	}

	int cascadeCount = std::any_cast<int>(m_cascadeCountProp->Get(registry, entity));
	result = EditorProperty<int>("Shadow Cascade Count", cascadeCount, m_cascadeCountProp->Attributes()).Draw();
	if (result.Changed)
	{
		m_cascadeCountProp->Set(cascadeCount, registry, entity);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_cascadeCountProp, registry, entity);
	}
	if (result.EditEnded)
	{
		UndoSystem::EndPropertyEdit(registry, entity);
	}

	float cascadeDistribution = std::any_cast<float>(m_cascadeDistributionProp->Get(registry, entity));
	result = EditorProperty<float>("Shadow Cascade Distribution", cascadeDistribution, m_cascadeDistributionProp->Attributes()).Draw();
	if (result.Changed)
	{
		m_cascadeDistributionProp->Set(cascadeDistribution, registry, entity);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_cascadeDistributionProp, registry, entity);
	}
	if (result.EditEnded)
	{
		UndoSystem::EndPropertyEdit(registry, entity);
	}
}
