#include "Windows/EditorWindow.h"
#include <Core/EditorSystem.h>

void EditorWindow::Draw(entt::entity& selected, Scene& scene)
{
	ImGui::Begin(m_title, nullptr, m_showUnsaved ? m_flags | ImGuiWindowFlags_UnsavedDocument : m_flags);

	bool focused = ImGui::IsWindowFocused();
	if (focused && !m_wasFocused)
	{
		Event e;
		e.Type = EventType::WindowFocus;
		OnEvent(e);
	}
	else if (!focused && m_wasFocused)
	{
		Event e;
		e.Type = EventType::WindowFocusLost;
		OnEvent(e);
	}
	m_wasFocused = focused;

	m_windowSize = ImGui::GetContentRegionAvail();

	if (std::abs(m_windowSize.x - m_lastFrameWindowSize.x) > std::numeric_limits<float>::epsilon()
		|| std::abs(m_windowSize.y - m_lastFrameWindowSize.y) > std::numeric_limits<float>::epsilon())
	{
		Event e;
		e.Type = EventType::WindowResize;
		e.WindowResizeArgs.Width = m_windowSize.x;
		e.WindowResizeArgs.Height = m_windowSize.y;
		OnEvent(e);
	}

	m_lastFrameWindowSize = m_windowSize;


	DrawContent(selected, scene);
	ImGui::End();
}
