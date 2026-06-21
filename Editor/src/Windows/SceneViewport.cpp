#include "Windows/SceneViewport.h"

#include "Core/EditorSystem.h"
#include "Input/InputSystem.h"
#include <glm/gtx/matrix_decompose.hpp>

void SceneViewport::Open()
{
	EditorSystem::Get().OpenWindow<SceneViewport>();
}

void SceneViewport::Initialise()
{
	EditorSystem::Get().GetEditorCamTransform().Position = glm::vec3(4.0f, 4.0f, 4.0f);
	m_editorCamRotation = glm::vec3(-45.0f, 45.0f, 0.0f);
	EditorSystem::Get().GetEditorCamTransform().Rotation = glm::quat(glm::radians(m_editorCamRotation));
}

void SceneViewport::DrawContent(entt::entity& selected, Scene& scene)
{
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		m_capturingViewport = true;
		Window::Get().SetCursorMode(CursorMode::Disabled);
	}
	if (m_capturingViewport)
	{
		float lookSpeed = 0.08f;
		glm::vec2 mouseDelta = InputSystem::Get().MouseDelta();

		m_editorCamRotation.x += -mouseDelta.y * lookSpeed;
		m_editorCamRotation.y += -mouseDelta.x * lookSpeed;
		EditorSystem::Get().GetEditorCamTransform().Rotation = glm::quat(glm::radians(m_editorCamRotation));

		auto moveAction = InputSystem::Get().GetAction("Move");
		if (!moveAction) { return; }

		float flightSpeed = m_editorFlyCamSpeed;
		if (InputSystem::Get().IsKeyDown(InputBinding::LeftShift))
		{
			flightSpeed *= 2.0f;
		}
		glm::vec2 inputVector = moveAction->Get<glm::vec2>();
		glm::vec3 position = EditorSystem::Get().GetEditorCamTransform().Position;
		float curSpeedX = inputVector.x * flightSpeed * Window::Get().DeltaTime();
		float curSpeedZ = inputVector.y * flightSpeed * Window::Get().DeltaTime();
		glm::vec3 moveDirection = (EditorSystem::Get().GetEditorCamTransform().Forward() * curSpeedZ) + (EditorSystem::Get().GetEditorCamTransform().Right() * -curSpeedX);
		position += moveDirection;

		EditorSystem::Get().GetEditorCamTransform().Position = position;
	}

	if (m_capturingViewport && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
	{
		m_capturingViewport = false;
		Window::Get().SetCursorMode(CursorMode::Visible);
	}

	uint32_t textureID = EditorSystem::Get().GetSceneRenderTarget()->GetTarget()->GetID();
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImGui::ImageWithBg((void*)(intptr_t)textureID, m_windowSize, uv0, uv1, ImVec4(0, 0, 0, 1));

	DrawGizmosToViewport(selected, scene);
}

void SceneViewport::OnEvent(Event& e)
{
	if (e.Type == EventType::WindowResize)
	{
		EditorSystem::Get().GetSceneRenderTarget()->Resize(glm::ivec2(e.WindowResizeArgs.Width, e.WindowResizeArgs.Height));
		EditorSystem::Get().RecalcCamProjection();
	}
}

void SceneViewport::DrawGizmosToViewport(entt::entity& selected, Scene& scene)
{
	Gizmos::WasManipulatedThisFrame = false;

	if (selected == entt::null) { return; }

	glm::mat4 view = EditorSystem::Get().GetEditorCameraData().View;
	glm::mat4 proj = EditorSystem::Get().GetEditorCameraData().Projection;

	auto& transform = scene.Registry.get<Transform>(selected);
	glm::mat4 matrix = transform.GetTransformMatrix();

	bool isUsing = ImGuizmo::IsUsing();
	static glm::mat4 beforeMatrix;
	if (isUsing && !m_wasUsingGizmo)
	{
		beforeMatrix = matrix;
	}

	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImGuizmo::SetRect(m_windowPos.x, m_windowPos.y, m_windowSize.x, m_windowSize.y);
	bool setEulerCache = false;
	if (ImGuizmo::Manipulate(&view[0][0], &proj[0][0], Gizmos::CurrentGizmoOperation, Gizmos::CurrentGizmoMode, &matrix[0][0], NULL, NULL))
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(matrix, transform.Scale, transform.Rotation, transform.Position, skew, perspective);
		EditorSystem::Get().SetSceneDirty();
		Gizmos::WasManipulatedThisFrame = true;
	}
	if (!isUsing && m_wasUsingGizmo)
	{
		glm::mat4 before = beforeMatrix;
		glm::mat4 after = matrix;
		UndoCommand undo;
		undo.Action = [after, &transform]()
		{
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(after, transform.Scale, transform.Rotation, transform.Position, skew, perspective);
		};
		undo.UndoAction = [before, &transform]()
		{
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(before, transform.Scale, transform.Rotation, transform.Position, skew, perspective);
		};
		UndoSystem::AddUndoCommand(undo);
	}
	m_wasUsingGizmo = isUsing;
}
