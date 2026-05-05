#include "TransformInspector.h"


void TransformInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
	if (!m_EulerCached)
	{
		m_EulerCache = glm::degrees(m_Component->GetTarget<Transform>(registry, entity)->EulerAngles());
		m_CurrentEuler = m_EulerCache;
		m_EulerCached = true;
	}

	if (auto positionProp = m_Component->GetProperty("Position"))
	{
		glm::vec3 position = std::any_cast<glm::vec3>(positionProp->Get(registry, entity));
		if (EditorProperty<glm::vec3>("Position", position).Draw())
		{
			positionProp->Set(position, registry, entity);
		}
	}
	if (auto rotationProp = m_Component->GetProperty("Rotation"))
	{
		if (EditorProperty<glm::vec3>("Rotation", m_CurrentEuler).Draw())
		{
			glm::vec3 radianAngles = glm::radians(m_CurrentEuler);

			glm::quat x = glm::angleAxis(radianAngles.x, glm::vec3(1, 0, 0));
			glm::quat y = glm::angleAxis(radianAngles.y, glm::vec3(0, 1, 0));
			glm::quat z = glm::angleAxis(radianAngles.z, glm::vec3(0, 0, 1));

			rotationProp->Set(glm::normalize(x * y * z), registry, entity);
			m_EulerCache = m_CurrentEuler;
		}
	}
	if (auto scaleProp = m_Component->GetProperty("Scale"))
	{
		glm::vec3 scale = std::any_cast<glm::vec3>(scaleProp->Get(registry, entity));
		if (EditorProperty<glm::vec3>("Scale", scale).Draw())
		{
			scaleProp->Set(scale, registry, entity);
		}
	}
}
