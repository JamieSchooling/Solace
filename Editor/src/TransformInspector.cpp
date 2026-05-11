#include "TransformInspector.h"


void TransformInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
	if (!m_eulerCached)
	{
		m_eulerCache = glm::degrees(m_component->GetTarget<Transform>(registry, entity)->EulerAngles());
		m_currentEuler = m_eulerCache;
		m_eulerCached = true;
	}

	if (auto positionProp = m_component->GetProperty("Position"))
	{
		glm::vec3 position = std::any_cast<glm::vec3>(positionProp->Get(registry, entity));
		if (EditorProperty<glm::vec3>("Position", position).Draw())
		{
			positionProp->Set(position, registry, entity);
		}
	}
	if (auto rotationProp = m_component->GetProperty("Rotation"))
	{
		if (EditorProperty<glm::vec3>("Rotation", m_currentEuler).Draw())
		{
			glm::vec3 radianAngles = glm::radians(m_currentEuler);

			glm::quat x = glm::angleAxis(radianAngles.x, glm::vec3(1, 0, 0));
			glm::quat y = glm::angleAxis(radianAngles.y, glm::vec3(0, 1, 0));
			glm::quat z = glm::angleAxis(radianAngles.z, glm::vec3(0, 0, 1));

			rotationProp->Set(glm::normalize(x * y * z), registry, entity);
			m_eulerCache = m_currentEuler;
		}
	}
	if (auto scaleProp = m_component->GetProperty("Scale"))
	{
		glm::vec3 scale = std::any_cast<glm::vec3>(scaleProp->Get(registry, entity));
		if (EditorProperty<glm::vec3>("Scale", scale).Draw())
		{
			scaleProp->Set(scale, registry, entity);
		}
	}
}
