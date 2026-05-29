#include "Core/EditorSystem.h"

#include <Scenes/Scene.h>
#include <imgui.h>
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
#include <Input/InputSystem.h>
#include <Menu/FileMenu.h>
#include <Core/Editor.h>
#include <Scenes/SceneSerialiser.h>

void EditorSystem::Start(const SubsystemParams& params)
{
	const EditorSystemProps& props = static_cast<const EditorSystemProps&>(params);

	GLFWwindow* window = props.GLFWInstance;
	props.EventSystem->AddListener(this);
	
	// For later use when viewport windows are implemented
	//m_gameRenderTarget = props.GameRenderTarget;

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

	OpenWindow<SceneHierarchy>();
	OpenWindow<InspectorWindow>();
	OpenWindow<AssetBrowser>();

	InputSystem::Get().AddFourComponentAction("Move", InputBinding::W, InputBinding::A, InputBinding::S, InputBinding::D);
	m_editorCamTransform.Position = glm::vec3(4.0f, 4.0f, 4.0f);
	m_editorCamRotation = glm::vec3(-45.0f, 45.0f, 0.0f);
	m_editorCamTransform.Rotation = glm::quat(glm::radians(m_editorCamRotation));


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

void EditorSystem::OnAppUpdate()
{
	if (Editor::CurrentState() != EditorState::Edit) { return; }

	if (InputSystem::Get().WasKeyPressedThisFrame(InputBinding::MouseButtonRight))
	{
		Window::Get().SetCursorMode(CursorMode::Disabled);
	}
	else if (InputSystem::Get().WasKeyReleasedThisFrame(InputBinding::MouseButtonRight))
	{
		Window::Get().SetCursorMode(CursorMode::Visible);
	}

	if (InputSystem::Get().IsKeyDown(InputBinding::MouseButtonRight))
	{
		float lookSpeed = 0.08f;
		glm::vec2 mouseDelta = InputSystem::Get().MouseDelta();

		m_editorCamRotation.x += -mouseDelta.y * lookSpeed;
		m_editorCamRotation.y += -mouseDelta.x * lookSpeed;
		m_editorCamTransform.Rotation = glm::quat(glm::radians(m_editorCamRotation));

		auto moveAction = InputSystem::Get().GetAction("Move");
		if (!moveAction) { return; }

		float flightSpeed = m_editorFlyCamSpeed;
		if (InputSystem::Get().IsKeyDown(InputBinding::LeftShift))
		{
			flightSpeed *= 2.0f;
		}
		glm::vec2 inputVector = moveAction->Get<glm::vec2>();
		glm::vec3 position = m_editorCamTransform.Position;
		float curSpeedX = inputVector.x * flightSpeed * Window::Get().DeltaTime();
		float curSpeedZ = inputVector.y * flightSpeed * Window::Get().DeltaTime();
		glm::vec3 moveDirection = (m_editorCamTransform.Forward() * curSpeedZ) + (m_editorCamTransform.Right() * -curSpeedX);
		position += moveDirection;

		m_editorCamTransform.Position = position;
	}

	
}

void EditorSystem::PostAppUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);
	ImGuizmo::BeginFrame();

	DrawMenuBar();
	DrawToolbar();

	ImGui::ShowDemoWindow();

	ImGui::Begin("Debug Window");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	Scene& scene = SceneSystem::Get().GetActiveScene();
	for (auto&& window : m_windows)
	{
		window->Draw(m_selectedEntity, scene);
	}

	if (Editor::CurrentState() == EditorState::Edit)
	{
		for (auto&& window : m_windows)
		{
			window->DrawGizmos(m_selectedEntity, m_editorCamera, m_editorCamTransform, scene);
		}
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

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteAlways))
	{
		FileMenu::Save();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorSystem::OnEvent(Event& e)
{
	if (e.Type == EventType::WindowResize)
	{
		if (e.WindowResizeArgs.Width > 0 && e.WindowResizeArgs.Height > 0)
		{
			m_editorCamera.RecalculateProjection();
		}
	}

	if (e.Type == EventType::SceneLoad)
	{
		Scene& scene = SceneSystem::Get().GetActiveScene();
		for (auto&& window : m_windows)
		{
			window->Initialise(scene);
		}
	}
	for (auto&& window : m_windows)
	{
		window->OnEvent(e);
	}

	if (e.Type == EventType::InputKey && ImGui::GetIO().WantCaptureKeyboard)
	{
		e.Handled = true;
	}
	else if (e.Type == EventType::InputMouseButton && ImGui::GetIO().WantCaptureMouse)
	{
		e.Handled = true;
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
	ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

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
