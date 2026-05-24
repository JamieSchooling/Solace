#include "Inspectors/TransformInspector.h"

#include <ImGuizmo.h>
#include <Input/InputSystem.h>

#include <glm/gtx/matrix_decompose.hpp>

void TransformInspector::Initialise(entt::registry& r, entt::entity e)
{
	m_transform = m_component->GetTarget<Transform>(r, e);

	m_eulerCache = glm::degrees(m_transform->EulerAngles());

	if (auto eulerProp = m_component->GetProperty("m_EulerAngles"))
	{
		glm::vec3 euler = std::any_cast<glm::vec3>(eulerProp->Get(r, e));
		if (euler != glm::vec3(0.0f) && euler != m_eulerCache) { m_eulerCache = euler; } // Get saved euler angles if they exist
	}
	m_currentEuler = m_eulerCache;
}

void TransformInspector::DrawInspector(entt::registry& registry, entt::entity entity)
{
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
			if (auto eulerProp = m_component->GetProperty("m_EulerAngles"))
			{
				eulerProp->Set(m_eulerCache, registry, entity);
			}
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

void TransformInspector::DrawGizmos(Camera& editorCamera, Transform& editorCamTransform, entt::registry& registry, entt::entity entity)
{
	glm::mat4 view = editorCamera.GetView(editorCamTransform);
	glm::mat4 proj = editorCamera.GetProjection();
	
	static ImGuizmo::OPERATION s_currentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE s_currentGizmoMode(ImGuizmo::WORLD);

	if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::W))
	{
		s_currentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	else if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::E))
	{
		s_currentGizmoOperation = ImGuizmo::ROTATE;
	}
	else if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::R))
	{
		s_currentGizmoOperation = ImGuizmo::SCALE;
	}

	if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::X))
	{
		s_currentGizmoMode = s_currentGizmoMode == ImGuizmo::WORLD ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
	}

	glm::mat4 matrix = m_transform->GetTransformMatrix();
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	bool setEulerCache = false;
	if (ImGuizmo::Manipulate(&view[0][0], &proj[0][0], s_currentGizmoOperation, s_currentGizmoMode, &matrix[0][0], NULL, NULL) 
		&& s_currentGizmoOperation == ImGuizmo::ROTATE)
	{
		setEulerCache = true;
	}

	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(matrix, m_transform->Scale, m_transform->Rotation, m_transform->Position, skew, perspective);

	if (setEulerCache)
	{
		float matrixTranslation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0], matrixTranslation, &m_currentEuler.x, matrixScale);
		m_eulerCache = m_currentEuler; 
		if (auto eulerProp = m_component->GetProperty("m_EulerAngles"))
		{
			eulerProp->Set(m_eulerCache, registry, entity);
		}
	}
}
