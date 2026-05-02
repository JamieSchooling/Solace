#pragma once
#include <memory>
#include <Rendering/FBO.h>
#include <imgui.h>

class ViewportWindow
{
public:
	ViewportWindow(const char* title, std::shared_ptr<FBO> framebuffer);

	void Draw();

private:
	const char* m_Title;
	std::shared_ptr<FBO> m_Framebuffer;
	ImVec2 m_WindowSize;
	ImVec2 m_LastFrameWindowSize;
};