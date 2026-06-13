#include "Inspectors/CameraInspector.h"
#include "Inspectors/EditorProperty.h"

void CameraInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
	auto projTypeProp = m_component->GetProperty("ProjectionType");
	EnumInfo projectionInfo = std::any_cast<EnumInfo>(projTypeProp->Get(registry, entity));
	if (EditorProperty<EnumInfo>("Projection Type", projectionInfo).Draw())
	{
		projTypeProp->Set(projectionInfo.CurrentValue, registry, entity);
		m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
	}

	auto projectionType = magic_enum::enum_cast<CameraProjectionType>(projectionInfo.CurrentValue);
	if (projectionType.has_value())
	{
		if (projectionType.value() == CameraProjectionType::Perspective)
		{
			auto fovProp = m_component->GetProperty("FOV");
			float fov = std::any_cast<float>(fovProp->Get(registry, entity));
			if (EditorProperty<float>("FOV", fov).Draw())
			{
				fovProp->Set(fov, registry, entity);
				m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
			}
		}
		else if (projectionType.value() == CameraProjectionType::Orthographic)
		{
			auto sizeProp = m_component->GetProperty("Size");
			float size = std::any_cast<float>(sizeProp->Get(registry, entity));
			if (EditorProperty<float>("Size", size).Draw())
			{
				sizeProp->Set(size, registry, entity);
				m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
			}
		}
	}

	if (auto nearProp = m_component->GetProperty("Near"))
	{
		float near = std::any_cast<float>(nearProp->Get(registry, entity));
		if (EditorProperty<float>("Near", near).Draw())
		{
			nearProp->Set(near, registry, entity);
			m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
		}
	}

	if (auto farProp = m_component->GetProperty("Far"))
	{
		float far = std::any_cast<float>(farProp->Get(registry, entity));
		if (EditorProperty<float>("Far", far).Draw())
		{
			farProp->Set(far, registry, entity);
			m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
		}
	}
}
