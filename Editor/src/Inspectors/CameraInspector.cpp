#include "Inspectors/CameraInspector.h"

void CameraInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
	if (auto fovProp = m_component->GetProperty("FOV"))
	{
		float fov = std::any_cast<float>(fovProp->Get(registry, entity));
		if (EditorProperty<float>("FOV", fov).Draw())
		{
			fovProp->Set(fov, registry, entity);
			m_component->GetTarget<Camera>(registry, entity)->RecalculateProjection();
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
