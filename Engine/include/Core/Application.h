#pragma once

#include <vector>
#include "Core/Subsystem.h"

class Application
{
public:
	void ExecuteLifecycle();

	virtual void Initialise() = 0;
	virtual void Run() = 0;
	virtual void Shutdown() = 0;
protected:
	template<typename T, typename = std::enable_if_t<std::is_base_of<SingletonSubsystem<T>, T>::value>>
	void AddSubsystem(const SubsystemParams& params = {})
	{
		T& subsystem = T::Get();
		subsystem.Start(params);
		m_Subsystems.push_back(&subsystem);
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of<SingletonSubsystem<T>, T>::value>>
	void RemoveSubsystem()
	{
		T& subsystem = T::Get();
		subsystem.Shutdown();
		m_Subsystems.erase(std::find(m_Subsystems.begin(), m_Subsystems.end(), &subsystem));
	}
	void UpdateSubsystems();
private:
	std::vector<Subsystem*> m_Subsystems;
};

extern Application* CreateApplication();