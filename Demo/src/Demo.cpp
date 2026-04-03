#include "Demo.h"

#include <Core/EntryPoint.h>
#include <Events/EventSystem.h>
#include <Rendering/Window.h>

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

	auto renderQueue = std::make_shared<RenderQueue>(); // Could create in a better place

	/*
	* Concept:
	* On update, SceneManager processes current scene, submits to render queue
	* 
	* SceneManagerProps props;
	* props.renderQueue = renderQueue;
	* 
	* AddSubsystem<SceneManager>(props);
	*/

	{
		RenderSystemProps props;
		props.renderQueue = renderQueue;

		AddSubsystem<RenderSystem>(props);
	}
	
	
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
