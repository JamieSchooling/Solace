#include "Inspectors/TransformInspector.h"
#include "Inspectors/EditorProperty.h"

#include <ImGuizmo.h>
#include <Input/InputSystem.h>

#include "Windows/Gizmos.h"

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
		EditResult result = EditorProperty<glm::vec3>("Position", position).Draw();
		if (result.Changed)
		{
			positionProp->Set(position, registry, entity);
		}
		if (result.EditStarted)
		{
			UndoSystem::BeginPropertyEdit(positionProp, registry, entity);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndPropertyEdit(registry, entity);
		}
	}
	if (auto rotationProp = m_component->GetProperty("Rotation"))
	{
		EditResult result = EditorProperty<glm::vec3>("Rotation", m_currentEuler).Draw();
		if (result.Changed)
		{
			glm::vec3 radianAngles = glm::radians(m_currentEuler);

			glm::quat x = glm::angleAxis(radianAngles.x, glm::vec3(1, 0, 0));
			glm::quat y = glm::angleAxis(radianAngles.y, glm::vec3(0, 1, 0));
			glm::quat z = glm::angleAxis(radianAngles.z, glm::vec3(0, 0, 1));

			rotationProp->Set(glm::normalize(x * y * z), registry, entity); 
			if (auto eulerProp = m_component->GetProperty("m_EulerAngles"))
			{
				eulerProp->Set(m_currentEuler, registry, entity);
			}
		}
		if (result.EditStarted)
		{
			m_eulerCache = m_currentEuler;
			UndoSystem::BeginPropertyEdit(rotationProp, registry, entity);
		}
		if (result.EditEnded)
		{
			auto before = m_eulerCache;
			m_eulerCache = m_currentEuler;
			auto after = m_eulerCache;
			UndoSystem::EndPropertyEdit(registry, entity, [this, &registry, entity, before, after](bool isUndo)
			{
				if (isUndo)
				{
					m_currentEuler = before;
				}
				else
				{
					m_currentEuler = after;
				}
			});
		}
	}
	if (auto scaleProp = m_component->GetProperty("Scale"))
	{
		glm::vec3 scale = std::any_cast<glm::vec3>(scaleProp->Get(registry, entity));
		EditResult result = EditorProperty<glm::vec3>("Scale", scale).Draw();
		if (result.Changed)
		{
			scaleProp->Set(scale, registry, entity);
		}
		if (result.EditStarted)
		{
			UndoSystem::BeginPropertyEdit(scaleProp, registry, entity);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndPropertyEdit(registry, entity);
		}
	}
}

void TransformInspector::EndFrame(entt::registry& registry, entt::entity entity)
{
	if (!Gizmos::CurrentGizmoOperation == ImGuizmo::ROTATE || !Gizmos::WasManipulatedThisFrame) { return; }

	glm::mat4 matrix = m_transform->GetTransformMatrix();

	float matrixTranslation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0], matrixTranslation, &m_currentEuler.x, matrixScale);
	m_eulerCache = m_currentEuler;
	if (auto eulerProp = m_component->GetProperty("m_EulerAngles"))
	{
		eulerProp->Set(m_eulerCache, registry, entity);
	}
}

void TransformInspector::DrawGizmos(entt::registry& registry, entt::entity entity)
{
	if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::W))
	{
		Gizmos::CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	else if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::E))
	{
		Gizmos::CurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	else if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::R))
	{
		Gizmos::CurrentGizmoOperation = ImGuizmo::SCALE;
	}

	if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::X))
	{
		Gizmos::CurrentGizmoMode = Gizmos::CurrentGizmoMode == ImGuizmo::WORLD ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
	}
}
