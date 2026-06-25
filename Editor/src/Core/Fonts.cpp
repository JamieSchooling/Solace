#include "Core/Fonts.h"
#include <Core/Application.h>

ImFont* Fonts::Default = nullptr;
ImFont* Fonts::SemiBold = nullptr;
ImFont* Fonts::Bold = nullptr;

void Fonts::Load()
{
	ImGuiIO& io = ImGui::GetIO();

	Default = io.Fonts->AddFontFromFileTTF((Application::GetResourcePath() / "Fonts" / "RobotoMono-Regular.ttf").string().c_str(), 16.0f);
	SemiBold = io.Fonts->AddFontFromFileTTF((Application::GetResourcePath() / "Fonts" / "RobotoMono-Medium.ttf").string().c_str(), 16.0f);
	Bold = io.Fonts->AddFontFromFileTTF((Application::GetResourcePath() / "Fonts" / "RobotoMono-SemiBold.ttf").string().c_str(), 16.0f);
}