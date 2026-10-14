#include "Windows/GameViewport.h"

#include "Core/EditorSystem.h"
#include "Rendering/RenderSystem.h"

#include <variant>

void GameViewport::Open()
{
	EditorSystem::Get().OpenWindow<GameViewport>();
}

void GameViewport::DrawContent(entt::entity& selected, Scene& scene)
{
	uint32_t textureID = std::get<std::shared_ptr<Texture>>(RenderSystem::Get().GetDefaultFramebuffer()->GetTarget())->GetID();
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImGui::ImageWithBg((void*)(intptr_t)textureID, m_windowSize, uv0, uv1, ImVec4(0, 0, 0, 1));
}

void GameViewport::OnEvent(Event& e)
{
	if (e.Type == EventType::WindowResize)
	{
		RenderSystem::Get().GetDefaultFramebuffer()->Resize(glm::ivec2(e.WindowResizeArgs.Width, e.WindowResizeArgs.Height));
		SceneSystem::Get().GetActiveScene().Registry.get<Camera>(SceneSystem::Get().GetActiveScene().MainCamera).RecalculateProjection({m_windowSize.x, m_windowSize.y});
	}
	else if (e.Type == EventType::SceneLoad)
	{
		SceneSystem::Get().GetActiveScene().Registry.get<Camera>(SceneSystem::Get().GetActiveScene().MainCamera).RecalculateProjection({m_windowSize.x, m_windowSize.y});
	}
}
