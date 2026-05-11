#pragma once

#include <vector>
#include "Core/Subsystem.h"

namespace std::filesystem {
	class path;
}

class Application
{
public:
	void ExecuteLifecycle();

	virtual void Initialise() = 0;
	virtual void Run() = 0;
	virtual void Shutdown() = 0;

	static std::filesystem::path GetResourcePath();
	static std::filesystem::path GetConfigPath();
	static std::filesystem::path GetDataPath();
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
		subsystem.Shutdown();
		m_subsystems.erase(std::find(m_subsystems.begin(), m_subsystems.end(), &subsystem));
	}
	void UpdateSubsystems();
private:
	std::vector<Subsystem*> m_subsystems;
};

extern Application* CreateApplication();