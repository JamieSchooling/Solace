#include "Windows/GameViewport.h"

#include "Core/EditorSystem.h"

void GameViewport::Open()
{
	EditorSystem::Get().OpenWindow<GameViewport>();
}

void GameViewport::DrawContent(entt::entity& selected, Scene& scene)
{
	uint32_t textureID = EditorSystem::Get().GetGameRenderTarget()->GetTarget()->GetID();
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImGui::ImageWithBg((void*)(intptr_t)textureID, m_windowSize, uv0, uv1, ImVec4(0, 0, 0, 1));
}

void GameViewport::OnEvent(Event& e)
{
	if (e.Type == EventType::WindowResize)
	{
		EditorSystem::Get().GetGameRenderTarget()->Resize(glm::ivec2(e.WindowResizeArgs.Width, e.WindowResizeArgs.Height));
		SceneSystem::Get().GetActiveScene().Registry.get<Camera>(SceneSystem::Get().GetActiveScene().MainCamera).RecalculateProjection({m_windowSize.x, m_windowSize.y});
	}
	else if (e.Type == EventType::SceneLoad)
	{
		SceneSystem::Get().GetActiveScene().Registry.get<Camera>(SceneSystem::Get().GetActiveScene().MainCamera).RecalculateProjection({m_windowSize.x, m_windowSize.y});
	}
}
