#include "ViewportWindow.h"
#include <iostream>

ViewportWindow::ViewportWindow(const char* title, std::shared_ptr<FBO> framebuffer)
	: m_Title(title), m_Framebuffer(framebuffer)
{
	m_WindowSize = ImVec2(0, 0);
	m_LastFrameWindowSize = m_WindowSize;
}

void ViewportWindow::Draw()
{
	ImGui::Begin(m_Title);
	m_WindowSize = ImGui::GetContentRegionAvail();

	// Window resized
	/*if (std::abs(m_WindowSize.x - m_LastFrameWindowSize.x) > std::numeric_limits<float>::epsilon()
		|| std::abs(m_WindowSize.y - m_LastFrameWindowSize.y) > std::numeric_limits<float>::epsilon())
	{
		m_Framebuffer->Resize({ m_WindowSize.x, m_WindowSize.y });
	}*/

	uint32_t textureID = m_Framebuffer->GetTarget()->GetID();
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImGui::Image((void*)(intptr_t)textureID, m_WindowSize, uv0, uv1);
	ImGui::End();

	m_LastFrameWindowSize = m_WindowSize;
}
