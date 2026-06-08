#include "Core/Editor.h"

#include <Core/EntryPoint.h>
#include <Assets/AssetRegistry.h>
#include <Events/EventSystem.h>
#include <Rendering/Window.h>
#include <Rendering/FrameRenderData.h>
#include <numeric>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Scenes/SceneSystem.h>
#include <Input/InputSystem.h>

#include <nfd.h>

#include "Core/EditorSystem.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Application* CreateApplication()
{
	return new Editor();
}

void Editor::Initialise(std::vector<std::string> args)
{
	AddSubsystem<EventSystem>(UpdatePhase::Always);

	{
		WindowProps props;
		props.Title = "Solace Editor";
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<Window>(UpdatePhase::Always, props);
	}

	NFD_Init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(Window::Get().GetGLFWInstance(), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	SetImGuiTheme();

	RunProjectManager(args);
	if (!m_projectManager.IsProjectLoaded()) { return; }

	s_projectDirectoryPath = m_projectManager.GetProjectPath();
	s_projectAssetsPath = m_projectManager.GetProjectAssetsPath();
	s_startupScene = m_projectManager.GetStartupScene();
	Window::Get().SetTitle("Solace Editor - " + s_projectDirectoryPath.filename().string());

	{
		AssetRegistryProps props;
		props.Root = s_projectAssetsPath;
		props.RegistryFile = s_projectDirectoryPath / "Data" / ".assetregistry";
		AddSubsystem<AssetRegistry>(UpdatePhase::Always, props);
	}

	{
		InputSystemProps props;
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<InputSystem>(UpdatePhase::Always, props);
	}

	{
		SceneSystemProps props;
		props.EventSystem = &EventSystem::Get();
		props.StartupScene = s_startupScene;
		AddSubsystem<SceneSystem>(UpdatePhase::Always, props);
	}

	{
		AddSubsystem<RenderSystem>(UpdatePhase::Always);
	};
	m_gameViewTarget = std::make_shared<FBO>();
	m_editorViewTarget = std::make_shared<FBO>();

	{
		EditorSystemProps props;
		props.GLFWInstance = Window::Get().GetGLFWInstance();
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<EditorSystem>(UpdatePhase::Always, props);
	}
}

void Editor::Run()
{
	while (Window::Get().IsOpen())
	{
		PreUpdate();
		Update();

		FrameRenderData frame;
		frame.RenderQueue = SceneSystem::Get().GetRenderQueue();
		frame.Lights = SceneSystem::Get().GetLightData();
		frame.ShadowView = SceneSystem::Get().GetShadowView();
		frame.ShadowQueue = SceneSystem::Get().GetShadowQueue();

		if (s_state == EditorState::Edit)
		{
			RenderView editorView{ EditorSystem::Get().GetEditorCameraData(), m_editorViewTarget };
			frame.RenderViews.push_back(editorView);
		}
		else if (s_state == EditorState::Play)
		{
			RenderView gameView{ SceneSystem::Get().GetActiveScene().GetMainCameraData(), m_gameViewTarget };
			frame.RenderViews.push_back(gameView);
		}

		RenderSystem::Get().SetFrameRenderData(frame);
		PostUpdate();
		FinaliseUpdate();
	}
}

void Editor::Shutdown()
{
	RemoveSubsystem<EditorSystem>();
	RemoveSubsystem<RenderSystem>();
	RemoveSubsystem<SceneSystem>();
	RemoveSubsystem<InputSystem>();
	NFD_Quit();
	m_projectManager.SerialiseProjectData(s_startupScene);
	RemoveSubsystem<AssetRegistry>();
	RemoveSubsystem<Window>();
	RemoveSubsystem<EventSystem>();

}

bool Editor::ShouldUpdate(const RegisteredSubsystem& subsystem) const
{
	switch (s_state)
	{
	case EditorState::Edit:
		return subsystem.Phase == UpdatePhase::Always;
	case EditorState::Play:
		return subsystem.Phase == UpdatePhase::Always || subsystem.Phase == UpdatePhase::Simulation;
	}
}

void Editor::SetImGuiTheme()
{
	ImGui::GetIO().Fonts->AddFontFromFileTTF((Application::GetResourcePath() / "Fonts" / "RobotoMono-Regular.ttf").string().c_str(), 16.0f);

	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.4f;
	style.WindowPadding = ImVec2(10.0f, 10.0f);
	style.WindowRounding = 2.5f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(20.0f, 20.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 2.5f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 2.5f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(5.0f, 1.5f);
	style.FrameRounding = 2.5f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(5.0f, 5.0f);
	style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
	style.CellPadding = ImVec2(10.0f, 5.0f);
	style.IndentSpacing = 10.0f;
	style.ColumnsMinSpacing = 5.0f;
	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 2.5f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 2.5f;
	style.TabRounding = 2.5f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Left;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.99999f, 0.99999344f, 1.0f, 0.53648067f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06262777f, 0.07036594f, 0.08583689f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.06262777f, 0.07036594f, 0.08583689f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.20392157f, 0.23529412f, 0.26666668f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20392157f, 0.23529412f, 0.26666668f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.11031702f, 0.29684803f, 0.48497856f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803922f, 0.105882354f, 0.12156863f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.16909504f, 0.47708625f, 0.7296137f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.16862746f, 0.47843137f, 0.7294118f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.16862746f, 0.47843137f, 0.7294118f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.20392157f, 0.23529412f, 0.26666668f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.20392157f, 0.23529412f, 0.26666668f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.20392157f, 0.23529412f, 0.26666668f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.16862746f, 0.47843137f, 0.7294118f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.06262777f, 0.07036594f, 0.08583689f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.20392157f, 0.23529412f, 0.26666668f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.52156866f, 0.6f, 0.7019608f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.039215688f, 0.98039216f, 0.98039216f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.29019606f, 0.8263569f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.47264895f, 0.86676055f, 0.99607843f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.20392157f, 0.23529412f, 0.26666668f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.15686275f, 0.5882353f, 0.84705883f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6866953f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(9.9999e-7f, 9.999962e-7f, 1e-6f, 0.6866953f);
}

void Editor::RunProjectManager(std::vector<std::string> args)
{
	std::filesystem::path projectPath;

	for (auto arg : args)
	{
		if (ProjectManager::IsProjectFile(arg))
		{
			projectPath = arg;
		}
	}

	m_projectManager.Initialise(projectPath);

	while (!m_projectManager.IsProjectLoaded() && Window::Get().IsOpen())
	{
		PreUpdate();
		Update();
		m_projectManager.Update();
		PostUpdate();
		FinaliseUpdate();
	}
}
