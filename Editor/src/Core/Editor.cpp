#include "Core/Editor.h"

#include <Core/EntryPoint.h>
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
	AddSubsystem<EventSystem>();

	{
		WindowProps props;
		props.Title = "Solace Editor";
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<Window>(props);
	}

	NFD_Init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(Window::Get().GetGLFWInstance(), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	RunProjectManager(args);
	if (!m_projectManager.IsProjectLoaded()) { return; }

	{
		InputSystemProps props;
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<InputSystem>(props);
	}

	{
		SceneSystemProps props;
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<SceneSystem>(props);
	}

	{
		AddSubsystem<RenderSystem>();
	};
	m_gameViewTarget = std::make_shared<FBO>();
	m_editorViewTarget = std::make_shared<FBO>();

	{
		EditorSystemProps props;
		props.GLFWInstance = Window::Get().GetGLFWInstance();
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<EditorSystem>(props);
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

		//RenderView gameView{ SceneSystem::Get().GetActiveScene().GetMainCameraData(), m_gameViewTarget };
		//frame.RenderViews.push_back(gameView);
		RenderView editorView{ EditorSystem::Get().GetEditorCameraData(), m_editorViewTarget};
		frame.RenderViews.push_back(editorView);

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
	RemoveSubsystem<Window>();
	RemoveSubsystem<EventSystem>();
}

void Editor::RunProjectManager(std::vector<std::string> args)
{
	std::filesystem::path projectPath;

	for (auto arg : args)
	{
		if (arg.ends_with(".solaceproj"))
		{
			projectPath = arg;
			std::cout << projectPath << std::endl;
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
