#include "Core/EditorSystem.h"

#include <Scenes/Scene.h>
#include <Scenes/SceneSystem.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <Scenes/NameComponent.h>

#include <filesystem>
#include <Core/Application.h>
#include <Transform/Transform.h>

#include <nfd.h>
#include <Scenes/SceneSerialiser.h>
#include <Rendering/Camera.h>

#include <iostream>

#include "Windows/SceneHierarchy.h"
#include "Windows/InspectorWindow.h"

void EditorSystem::Start(const SubsystemParams& params)
{
	const EditorSystemProps& props = static_cast<const EditorSystemProps&>(params);

	GLFWwindow* window = props.GLFWInstance;
	props.EventSystem->AddListener(this);
	
	// For later use when viewport windows are implemented
	//m_gameRenderTarget = props.GameRenderTarget;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	io.IniFilename = nullptr;
	if (!std::filesystem::exists(Application::GetConfigPath() / "Layouts"))
	{
		std::filesystem::create_directory(Application::GetConfigPath() / "Layouts");
	}

	if (std::filesystem::exists(Application::GetConfigPath() / "Layouts" / "Active.ini"))
	{
		ImGui::LoadIniSettingsFromDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
		m_newLayout = LayoutOption::Active;
	}
	else
	{
		ImGui::LoadIniSettingsFromDisk((Application::GetResourcePath() / "Layouts" / "Default.ini").string().c_str());
		m_newLayout = LayoutOption::Default;
	}

	io.Fonts->AddFontFromFileTTF((Application::GetResourcePath()/"Fonts"/"RobotoMono-Regular.ttf").string().c_str(), 16.0f);
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	m_windows.push_back(std::make_unique<SceneHierarchy>());
	m_windows.push_back(std::make_unique<InspectorWindow>());

	Scene& scene = SceneSystem::Get().GetActiveScene();
	for (auto&& window : m_windows)
	{
		window->Initialise(scene);
	}
}

void EditorSystem::Shutdown()
{
	ImGui::SaveIniSettingsToDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
}

void EditorSystem::PreAppUpdate()
{
	HandleLayoutChange(); // Done on frame start to preserve docking in layout
}

void EditorSystem::OnAppUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);

	DrawMenuBar();

	ImGui::ShowDemoWindow();

	ImGui::Begin("Debug Window");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	Scene& scene = SceneSystem::Get().GetActiveScene();
	for (auto&& window : m_windows)
	{
		window->Draw(m_selectedEntity, scene);
	}

	// Framework is there to make game render to an ImGui window (Editor Viewport Window), 
	// for now I use a dockspace with a passthrough central node, with the scene rendered to the default framebuffer
	//ImGui::Begin("Game");
	//GLuint textureID = m_gameRenderTarget->GetTarget()->GetID();
	//ImVec2 uv0 = ImVec2(0.0f, 1.0f);  // Top-left (flipped)
	//ImVec2 uv1 = ImVec2(1.0f, 0.0f);  // Bottom-right (flipped)
	//ImVec2 imageSize = ImGui::GetContentRegionAvail();  // (width, height)
	//ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);
	//ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorSystem::OnEvent(Event& e)
{
	if (e.Type == EventType::SceneLoad)
	{
		Scene& scene = SceneSystem::Get().GetActiveScene();
		for (auto&& window : m_windows)
		{
			window->Initialise(scene);
		}
	}
}

void EditorSystem::HandleLayoutChange()
{
	if (m_newLayout == m_currentLayout) return;

	if (m_currentLayout == LayoutOption::Active)
	{
		ImGui::SaveIniSettingsToDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
	}

	switch (m_newLayout)
	{
	case LayoutOption::Default:
		ImGui::LoadIniSettingsFromDisk((Application::GetResourcePath() / "Layouts" / "Default.ini").string().c_str());
		break;
	case LayoutOption::Active:
		ImGui::LoadIniSettingsFromDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
		break;
	}

	m_currentLayout = m_newLayout;
}

void EditorSystem::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open"))
			{
				nfdchar_t* outPath = nullptr;
				NFD_OpenDialog("solace", nullptr, &outPath);
				if (outPath)
				{
					std::cout << outPath << std::endl;
					Scene scene;
					SceneSerialiser serialiser(scene);
					serialiser.DeserialiseFrom(outPath);
					SceneSystem::Get().LoadScene(scene);
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save As..."))
			{
				nfdchar_t* outPath = nullptr;
				NFD_SaveDialog("solace", nullptr, &outPath);
				if (outPath)
				{
					std::cout << outPath << std::endl;
					SceneSerialiser serialiser(SceneSystem::Get().GetActiveScene());
					serialiser.SerialiseTo(outPath);
				}
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
			if (ImGui::MenuItem("Default", nullptr, m_currentLayout == LayoutOption::Default))
			{
				m_newLayout = LayoutOption::Default;
			}

			if (ImGui::MenuItem("Active", nullptr, m_currentLayout == LayoutOption::Active))
			{
				m_newLayout = LayoutOption::Active;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
