#include "Windows/EditorWindow.h"

#include <imgui.h>

void EditorWindow::Draw(entt::entity& selected, Scene& scene)
{
	ImGui::Begin(m_title);
	DrawContent(selected, scene);
	ImGui::End();
}
