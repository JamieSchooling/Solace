#include "Core/Application.h"

#include "Core/Subsystem.h"

#if defined(_WIN32)
#include <Windows.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#endif

void Application::ExecuteLifecycle(std::vector<std::string> args)
{
	std::filesystem::current_path(Application::GetExecutableDirectory());
	Initialise(args);
	Run();
	Shutdown();
}

void Application::PreUpdate()
{
	for (auto subsystem : m_subsystems)
	{
		subsystem->PreAppUpdate();
	}
}

void Application::Update()
{
	for (auto subsystem : m_subsystems)
	{
		subsystem->OnAppUpdate();
	}
}

void Application::PostUpdate()
{
	for (auto subsystem : m_subsystems)
	{
		subsystem->PostAppUpdate();
	}
}

void Application::FinaliseUpdate()
{
	for (auto subsystem : m_subsystems)
	{
		subsystem->FinaliseAppUpdate();
	}
}

std::filesystem::path Application::GetResourcePath()
{
	return std::filesystem::path("./resources").make_preferred();
}

std::filesystem::path Application::GetExecutableDirectory()
{
#if defined(_WIN32)
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(nullptr, buffer, MAX_PATH);
	return std::filesystem::path(buffer).parent_path();

#elif defined(__linux__)
	char buffer[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	buffer[len] = '\0';
	return std::filesystem::path(buffer).parent_path();

#elif defined(__APPLE__)
	char buffer[1024];
	uint32_t size = sizeof(buffer);
	_NSGetExecutablePath(buffer, &size);
	return std::filesystem::path(buffer).parent_path();
#endif
}

#if defined(_WIN32)
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
