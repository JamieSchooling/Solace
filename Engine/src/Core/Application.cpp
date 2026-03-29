#include "Core/Application.h"

#include "Core/Subsystem.h"

void Application::ExecuteLifecycle()
{
	Initialise();
	Run();
	Shutdown();
}

void Application::UpdateSubsystems()
{
	for (auto subsystem : m_Subsystems)
	{
		subsystem->PreAppUpdate();
	}
	for (auto subsystem : m_Subsystems)
	{
		subsystem->OnAppUpdate();
	}
	for (auto subsystem : m_Subsystems)
	{
		subsystem->PostAppUpdate();
	}
}
