#include "Core/ProjectManager.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <Rendering/Window.h>
#include <Core/Application.h>

void ProjectManager::Initialise(std::filesystem::path projectPath)
{
	if (!projectPath.empty())
	{
		LoadProject(projectPath);
	}

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF((Application::GetResourcePath() / "Fonts" / "RobotoMono-Regular.ttf").string().c_str(), 16.0f);
}

void ProjectManager::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("Project Manager", nullptr, flags);
	ImGui::PushFont(nullptr, 32.0f);
	ImGui::TextUnformatted("Projects");
	ImGui::PopFont();

	if (ProjectEntry("Sample Project", "/Path/To/Project", ImVec2(ImGui::GetContentRegionAvail().x, 55)))
	{
		LoadProject("");
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ProjectManager::IsProjectLoaded()
{
	return m_isProjectLoaded;
}

void ProjectManager::LoadProject(std::filesystem::path projectPath)
{
	m_isProjectLoaded = true;
}

void ProjectManager::CreateProject(std::filesystem::path projectDirectory)
{
}

bool ProjectManager::ProjectEntry(const std::string& projectName, const std::string& projectPath, const ImVec2& size)
{
	ImGui::PushID(projectPath.c_str());

	ImGui::BeginChild("ProjectEntry", size, 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 avail = ImGui::GetContentRegionAvail();

	bool opened = ImGui::Selectable("##select", false, 0, avail);

	ImGui::SetCursorPos(ImVec2(10, 0));	

	ImGui::PushFont(nullptr, 20.0f);
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(projectName.c_str());
	ImGui::PopFont();

	ImGui::SetCursorPosX(10);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
	ImGui::TextWrapped("%s", projectPath.c_str());
	ImGui::PopStyleColor();

	ImGui::EndChild();

	ImGui::PopID();

	return opened;
}
