#include "EditorSystem.h"

#include <Scenes/Scene.h>
#include <Scenes/SceneSystem.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <Scenes/NameComponent.h>

#include <filesystem>
#include <Core/Application.h>

void EditorSystem::Start(const SubsystemParams& params)
{
	const EditorSystemProps& props = static_cast<const EditorSystemProps&>(params);

	GLFWwindow* window = props.GLFWInstance;
	
	// For later use when viewport windows are implemented
	//m_GameRenderTarget = props.GameRenderTarget;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	if (!std::filesystem::exists(Application::GetConfigPath() / "Layouts"))
	{
		std::filesystem::create_directory(Application::GetConfigPath() / "Layouts");
	}

	if (std::filesystem::exists(Application::GetConfigPath() / "Layouts" / "Active.ini"))
	{
		ImGui::LoadIniSettingsFromDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
		m_NewLayout = LayoutOption::Active;
	}
	else
	{
		ImGui::LoadIniSettingsFromDisk((Application::GetResourcePath() / "Layouts" / "Default.ini").string().c_str());
		m_NewLayout = LayoutOption::Default;
	}

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void EditorSystem::Shutdown()
{
	ImGui::SaveIniSettingsToDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
}

void EditorSystem::OnAppUpdate()
{
	HandleLayoutChange(); // Done on frame start to preserve docking in layout

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);
	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Menu Item")) 
			{
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Menu Item"))
			{
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Layout")) 
		{
			if (ImGui::MenuItem("Default", nullptr, m_CurrentLayout == LayoutOption::Default))
			{
				m_NewLayout = LayoutOption::Default;
			}

			if (ImGui::MenuItem("Active", nullptr, m_CurrentLayout == LayoutOption::Active))
			{
				m_NewLayout = LayoutOption::Active;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}


	ImGui::ShowDemoWindow();

	ImGui::Begin("Debug Window");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::End();

	// Framework is there to make game render to an ImGui window (Editor Viewport Window), 
	// for now I use a dockspace with a passthrough central node, with the scene rendered to the default framebuffer
	//ImGui::Begin("Game");
	//GLuint textureID = m_GameRenderTarget->GetTarget()->GetID();
	//ImVec2 uv0 = ImVec2(0.0f, 1.0f);  // Top-left (flipped)
	//ImVec2 uv1 = ImVec2(1.0f, 0.0f);  // Bottom-right (flipped)
	//ImVec2 imageSize = ImGui::GetContentRegionAvail();  // (width, height)
	//ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);
	//ImGui::End();

	ImGui::Begin("Hierarchy");
	Scene& scene = SceneSystem::Get().GetActiveScene();

	for (auto& entity : scene.Registry.view<entt::entity>())
	{
		std::string name = "Unnamed Entity";
		if (scene.Registry.all_of<NameComponent>(entity))
		{
			name = scene.Registry.get<NameComponent>(entity).Name;
		}
		ImGui::PushID((int)entity);
		if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorSystem::HandleLayoutChange()
{
	if (m_NewLayout == m_CurrentLayout) return;

	if (m_CurrentLayout == LayoutOption::Active)
	{
		ImGui::SaveIniSettingsToDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
	}

	switch (m_NewLayout)
	{
	case LayoutOption::Default:
		ImGui::LoadIniSettingsFromDisk((Application::GetResourcePath() / "Layouts" / "Default.ini").string().c_str());
		break;
	case LayoutOption::Active:
		ImGui::LoadIniSettingsFromDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
		break;
	}

	m_CurrentLayout = m_NewLayout;
}
