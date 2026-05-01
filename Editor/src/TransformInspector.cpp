#include "TransformInspector.h"

TransformInspector::TransformInspector(Transform& transform) : m_Transform(transform), ComponentInspector("Transform")
{
	m_Properties.push_back(std::make_shared<EditorProperty<glm::vec3>>("Position", m_Transform.Position));

	m_EulerCache = glm::degrees(transform.EulerAngles());
	m_CurrentEuler = m_EulerCache;
	m_Properties.push_back(std::make_shared<EditorProperty<glm::vec3>>("Rotation", m_CurrentEuler, [this](glm::vec3& v) { OnRotationEdit(v); }));
	m_Properties.push_back(std::make_shared<EditorProperty<glm::vec3>>("Scale", m_Transform.Scale));
}

void TransformInspector::OnRotationEdit(glm::vec3& euler)
{
	glm::vec3 radianAngles = glm::radians(euler);

	glm::quat x = glm::angleAxis(radianAngles.x, glm::vec3(1, 0, 0));
	glm::quat y = glm::angleAxis(radianAngles.y, glm::vec3(0, 1, 0));
	glm::quat z = glm::angleAxis(radianAngles.z, glm::vec3(0, 0, 1));
	m_Transform.Rotation = glm::normalize(x * y * z);
	m_EulerCache = euler;
}
