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
