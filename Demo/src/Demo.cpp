#include "Demo.h"

#include <Core/EntryPoint.h>
#include <Events/EventSystem.h>
#include <Rendering/Window.h>
#include <Rendering/FrameRenderData.h>
#include <numeric>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Scenes/SceneSystem.h>

Application* CreateApplication()
{
	return new DemoApp();
}

void DemoApp::Initialise()
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
	
}

void DemoApp::Run()
{
	while (Window::Get().IsOpen())
	{
		UpdateSubsystems();
	}
}

void DemoApp::Shutdown()
{
	RemoveSubsystem<Window>();
	RemoveSubsystem<EventSystem>();
}
