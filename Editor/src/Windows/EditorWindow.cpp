#include "Windows/EditorWindow.h"

#include <imgui.h>

void EditorWindow::Draw(entt::entity& selected, Scene& scene)
{
	ImGui::Begin(m_title, nullptr, m_showUnsaved ? ImGuiWindowFlags_UnsavedDocument : 0);

	bool focused = ImGui::IsWindowFocused();

	if (focused && !m_wasFocused)
	{
		Event e;
		e.Type = EventType::WindowFocus;
		OnEvent(e);
	}

	if (!focused && m_wasFocused)
	{
		Event e;
		e.Type = EventType::WindowFocusLost;
		OnEvent(e);
	}

	m_wasFocused = focused;

	DrawContent(selected, scene);
	ImGui::End();
}
