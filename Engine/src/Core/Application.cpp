#include "Core/Application.h"

#include "Core/Subsystem.h"

#include <filesystem>

void Application::ExecuteLifecycle()
{
	Initialise();
	Run();
	Shutdown();
}

void Application::UpdateSubsystems()
{
	for (auto subsystem : m_subsystems)
	{
		subsystem->PreAppUpdate();
	}
	for (auto subsystem : m_subsystems)
	{
		subsystem->OnAppUpdate();
	}
	for (auto subsystem : m_subsystems)
	{
		subsystem->PostAppUpdate();
	}
}


std::filesystem::path Application::GetResourcePath()
{
	return std::filesystem::path("./resources").make_preferred();
}

#ifdef _WIN32
#include <Windows.h>
#include <KnownFolders.h>
#include <ShlObj.h>
std::filesystem::path Application::GetConfigPath()
{
	PWSTR path = nullptr;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);

	auto configPath = std::filesystem::path(path ? path : L"") / "Solace";

	if (!std::filesystem::exists(configPath))
	{
		std::filesystem::create_directory(configPath);
	}

	return configPath;
}

std::filesystem::path Application::GetDataPath()
{
	PWSTR path = nullptr;
	SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path);

	auto dataPath = std::filesystem::path(path ? path : L"") / "Solace";

	if (!std::filesystem::exists(dataPath))
	{
		std::filesystem::create_directory(dataPath);
	}

	return dataPath;
}
#endif
