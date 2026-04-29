#include "Editor.h"

#include <Core/EntryPoint.h>
#include <Events/EventSystem.h>
#include <Rendering/Window.h>
#include <Rendering/FrameRenderData.h>
#include <numeric>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Scenes/SceneSystem.h>

#include "EditorSystem.h"

Application* CreateApplication()
{
	return new Editor();
}

void Editor::Initialise()
{
	AddSubsystem<EventSystem>();

	{
		WindowProps props;
		props.title = "Demo";
		props.eventSystem = &EventSystem::Get();
		AddSubsystem<Window>(props);
	}

	{
		SceneSystemProps props;
		props.eventSystem = &EventSystem::Get();
		AddSubsystem<SceneSystem>(props);
	}

	{
		RenderSystemProps props;
		props.renderData = &SceneSystem::Get().GetRenderData();
		AddSubsystem<RenderSystem>(props);
	};	
	
	{
		EditorSystemProps props;
		props.GLFWInstance = Window::Get().GetGLFWInstance();
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
	RemoveSubsystem<Window>();
	RemoveSubsystem<EventSystem>();
}
