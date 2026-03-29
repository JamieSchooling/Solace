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
	WindowProps props;
	props.title = "Demo";
	AddSubsystem<Window>(props);

	AddSubsystem<EventSystem>();
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
	RemoveSubsystem<EventSystem>();
	RemoveSubsystem<Window>();
}
