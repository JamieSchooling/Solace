#include "Windows/EditorWindow.h"
#include <Core/EditorSystem.h>

void EditorWindow::Draw(entt::entity& selected, Scene& scene)
{
	ImGui::Begin(m_title, nullptr, m_showUnsaved ? m_flags | ImGuiWindowFlags_UnsavedDocument : m_flags);
	m_windowPos = ImGui::GetCursorScreenPos();

	bool focused = ImGui::IsWindowFocused();
	if (focused && !m_wasFocused)
	{
		Event e;
		e.Type = EventType::WindowFocus;
		m_eventQueue.push_back(e);
	}
	else if (!focused && m_wasFocused)
	{
		Event e;
		e.Type = EventType::WindowFocusLost;
		m_eventQueue.push_back(e);
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
		m_eventQueue.push_back(e);
	}

	m_lastFrameWindowSize = m_windowSize;


	DrawContent(selected, scene);
	ImGui::End();
}

void EditorWindow::DispatchEvents()
{
	for (auto event : m_eventQueue)
	{
		OnEvent(event);
	}
	m_eventQueue.clear();
}

glm::vec2 EditorWindow::Position()
{
	return { m_windowPos.x, m_windowPos.y };
}

glm::vec2 EditorWindow::Size()
{
	return { m_windowSize.x, m_windowSize.y };
}

const char* EditorWindow::Title()
{
	return m_title;
}
