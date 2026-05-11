#include "Core/Editor.h"

#include <Core/EntryPoint.h>
#include <Events/EventSystem.h>
#include <Rendering/Window.h>
#include <Rendering/FrameRenderData.h>
#include <numeric>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Scenes/SceneSystem.h>

#include "Core/EditorSystem.h"

Application* CreateApplication()
{
	return new Editor();
}

void Editor::Initialise()
{
	AddSubsystem<EventSystem>();

	{
		WindowProps props;
		props.Title = "Solace Editor";
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<Window>(props);
	}

	{
		SceneSystemProps props;
		props.EventSystem = &EventSystem::Get();
		AddSubsystem<SceneSystem>(props);
	}

	{
		RenderSystemProps props;
		props.renderData = &SceneSystem::Get().GetRenderData();
		//props.renderTarget = std::make_shared<FBO>(glm::ivec2(Window::Get().GetWidth(), Window::Get().GetHeight()));
		AddSubsystem<RenderSystem>(props);
	};	
	
	{
		EditorSystemProps props;
		props.GLFWInstance = Window::Get().GetGLFWInstance();
		props.EventSystem = &EventSystem::Get();
		// For later use when viewport windows are implemented
		//props.GameRenderTarget = RenderSystem::Get().GetRenderTarget();
		AddSubsystem<EditorSystem>(props);
	}
}

void Editor::Run()
{
	while (Window::Get().IsOpen())
	{
		UpdateSubsystems();
	}
}

void Editor::Shutdown()
{
	RemoveSubsystem<EditorSystem>();
	RemoveSubsystem<RenderSystem>();
	RemoveSubsystem<SceneSystem>();
	RemoveSubsystem<Window>();
	RemoveSubsystem<EventSystem>();
}
