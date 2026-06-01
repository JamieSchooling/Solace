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
