#include "Core/EditorSystem.h"

#include <Scenes/Scene.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <Scenes/NameComponent.h>

#include <ImGuizmo.h>

#include <filesystem>
#include <Core/Application.h>

#include <iostream>

#include "Windows/SceneHierarchy.h"
#include "Windows/InspectorWindow.h"
#include "Windows/AssetBrowser.h"
#include "Windows/SceneViewport.h"
#include "Windows/GameViewport.h"
#include <Input/InputSystem.h>
#include <Menu/FileMenu.h>
#include <Core/Editor.h>
#include <Scenes/SceneSerialiser.h>

void EditorSystem::Start(const SubsystemParams& params)
{
	const EditorSystemProps& props = static_cast<const EditorSystemProps&>(params);

	GLFWwindow* window = props.GLFWInstance;
	props.EventSystem->AddListener(this);
	
	m_editorRenderTarget = props.EditorRenderTarget;
	m_gameRenderTarget = props.GameRenderTarget;

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

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

	OpenWindow<SceneHierarchy>();
	OpenWindow<InspectorWindow>();
	OpenWindow<AssetBrowser>();
	OpenWindow<GameViewport>();
	OpenWindow<SceneViewport>();

	InputSystem::Get().AddFourComponentAction("Move", InputBinding::W, InputBinding::A, InputBinding::S, InputBinding::D);

	m_playIcon = std::make_unique<Texture>(Application::GetResourcePath() / "Icons" / "PlayButton.png");
	m_stopIcon = std::make_unique<Texture>(Application::GetResourcePath() / "Icons" / "StopButton.png");
}

void EditorSystem::Shutdown()
{
	ImGui::SaveIniSettingsToDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
}

void EditorSystem::PreAppUpdate()
{
	HandleLayoutChange(); // Done on frame start to preserve docking in layout
}

void EditorSystem::PostAppUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode /*| ImGuiDockNodeFlags_NoDockingOverCentralNode*/);
	ImGuizmo::BeginFrame();

	DrawMenuBar();
	DrawToolbar();


	ImGui::Begin("Debug Window");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	Scene& scene = SceneSystem::Get().GetActiveScene();

	for (auto&& window : m_windows)
	{
		window->DrawGizmos(m_selectedEntity, scene);
	}

	for (auto&& window : m_windows)
	{
		window->Draw(m_selectedEntity, scene);
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteAlways))
	{
		FileMenu::Save();
		SetSceneDirty(false);
	}

	if (m_showSceneSaveModal)
	{
		DrawSceneSaveModal();
	}

	ImGui::Render(); 

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorSystem::FinaliseAppUpdate()
{
	Scene& scene = SceneSystem::Get().GetActiveScene();
	for (auto&& window : m_windows)
	{
		window->EndFrame(m_selectedEntity, scene);
	}

	for (auto&& window : m_windows)
	{
		window->DispatchEvents();
	}
}

void EditorSystem::OnEvent(Event& e)
{
	if (e.Type == EventType::WindowClose)
	{
		if (IsSceneDirty())
		{
			e.Handled = true;
			ShowSceneSaveModal();
		}
	}
	else if (e.Type == EventType::WindowResize)
	{
		if (e.WindowResizeArgs.Width > 0 && e.WindowResizeArgs.Height > 0)
		{
			m_editorCamera.RecalculateProjection();
		}
	}
	else if (e.Type == EventType::SceneLoad)
	{
		Scene& scene = SceneSystem::Get().GetActiveScene();
		for (auto&& window : m_windows)
		{
			window->Initialise(scene);
		}
	}
	else if (e.Type == EventType::InputKey && ImGui::GetIO().WantCaptureKeyboard)
	{
		e.Handled = true;
	}
	else if (e.Type == EventType::InputMouseButton && ImGui::GetIO().WantCaptureMouse)
	{
		e.Handled = true;
	}
	for (auto&& window : m_windows)
	{
		if (e.Handled) break;
		window->OnEvent(e);
	}
}

std::filesystem::path EditorSystem::GetCurrentlyOpenScene() const
{
	return m_currentlyOpenScene;
}

void EditorSystem::SetCurrentlyOpenScene(std::filesystem::path path)
{
	m_currentlyOpenScene = path;
}

void EditorSystem::SetSceneDirty(bool dirty)
{
	m_sceneDirty = dirty;
}

bool EditorSystem::IsSceneDirty() const
{
	return m_sceneDirty;
}

void EditorSystem::ShowSceneSaveModal()
{
	m_showSceneSaveModal = true;
}

void EditorSystem::RecalcCamProjection()
{
	m_editorCamera.RecalculateProjection(m_editorRenderTarget->GetSize());
}

std::shared_ptr<FBO> EditorSystem::GetSceneRenderTarget()
{
	return m_editorRenderTarget;
}

std::shared_ptr<FBO> EditorSystem::GetGameRenderTarget()
{
	return m_gameRenderTarget;
}

Transform& EditorSystem::GetEditorCamTransform()
{
	return m_editorCamTransform;
}

void EditorSystem::DrawSceneSaveModal()
{
	if (m_showSceneSaveModal)
	{
		ImGui::OpenPopup("Unsaved Changes");
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Unsaved Changes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are you sure you want to  quit?\nYou have unsaved changes.");
		ImGui::Separator();

		if (ImGui::Button("Save", ImVec2(120, 0)))
		{
			m_showSceneSaveModal = false;
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			FileMenu::Save();
			Window::Get().Close();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Don't Save", ImVec2(120, 0)))
		{
			m_showSceneSaveModal = false;
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			Window::Get().Close();
			return;
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_showSceneSaveModal = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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
		for (auto&& child : MenuRegistry::Root().Children)
		{
			DrawMenuNode(*child);
		}

		ImGui::EndMainMenuBar();
	}
}

void EditorSystem::DrawMenuNode(MenuNode& node)
{
	bool leaf = node.Children.empty();

	if (leaf)
	{
		bool hasCondition = node.SelectedCondition != nullptr;
		if (ImGui::MenuItem(node.Name.c_str(), nullptr, hasCondition ? node.SelectedCondition() : false))
		{
			if (node.Action)
			{
				node.Action();
			}
		}

		return;
	}

	if (ImGui::BeginMenu(node.Name.c_str()))
	{
		for (auto&& child : node.Children)
		{
			DrawMenuNode(*child);
		}

		ImGui::EndMenu();
	}
}

void EditorSystem::DrawToolbar()
{
	ImGuiWindowClass c;
	c.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoUndocking;
	ImGui::SetNextWindowClass(&c);
	ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove );

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

	auto& colours = ImGui::GetStyle().Colors;
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	const auto& buttonHovered = colours[ImGuiCol_ButtonHovered];
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
	const auto& buttonActive = colours[ImGuiCol_ButtonActive];
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

	float size = ImGui::GetContentRegionAvail().y;
	ImGui::SameLine((ImGui::GetContentRegionAvail().x * 0.5f) - (size * 0.5f));

	auto buttonContent = Editor::CurrentState() == EditorState::Edit ? m_playIcon->GetID() : m_stopIcon->GetID();

	if (ImGui::ImageButton("PlayButton", (ImTextureRef)buttonContent, {size, size}))
	{
		if (Editor::CurrentState() == EditorState::Edit)
		{
			FileMenu::Save();
			Editor::SetCurrentState(EditorState::Play);
			OpenWindow<GameViewport>();
		}
		else if (Editor::CurrentState() == EditorState::Play)
		{
			Editor::SetCurrentState(EditorState::Edit);
			Scene scene;
			SceneSerialiser s(scene);
			s.DeserialiseFrom(m_currentlyOpenScene);
			SceneSystem::Get().LoadScene(scene);
		}
	}

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
	ImGui::End();
}
