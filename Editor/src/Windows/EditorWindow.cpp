#include "Windows/EditorWindow.h"

#include <imgui.h>

void EditorWindow::Draw(entt::entity& selected, Scene& scene)
{
	ImGui::Begin(m_title, nullptr, m_showUnsaved ? ImGuiWindowFlags_UnsavedDocument : 0);
	DrawContent(selected, scene);
	ImGui::End();
}
