#pragma once

#include <string>
#include <vector>
#include "Core/Subsystem.h"

#include <filesystem>

class Application
{
public:
	void ExecuteLifecycle(std::vector<std::string> args);

	virtual void Initialise(std::vector<std::string> args) = 0;
	virtual void Run() = 0;
	virtual void Shutdown() = 0;

	static std::filesystem::path GetResourcePath();
	static std::filesystem::path GetConfigPath();
	static std::filesystem::path GetDataPath();
	static std::filesystem::path GetExecutableDirectory();
protected:
	template<typename T, typename = std::enable_if_t<std::is_base_of<SingletonSubsystem<T>, T>::value>>
	void AddSubsystem(const SubsystemParams& params = {})
	{
		T& subsystem = T::Get();
		subsystem.Start(params);
		m_subsystems.push_back(&subsystem);
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of<SingletonSubsystem<T>, T>::value>>
	void RemoveSubsystem()
	{
		T& subsystem = T::Get();
		auto it = std::find(m_subsystems.begin(), m_subsystems.end(), &subsystem);
		if (it != m_subsystems.end())
		{
			subsystem.Shutdown();
			m_subsystems.erase(it);
		}
	}

	void PreUpdate();
	void Update();
	void PostUpdate();
	void FinaliseUpdate();
private:
	std::vector<Subsystem*> m_subsystems;
};

extern Application* CreateApplication();