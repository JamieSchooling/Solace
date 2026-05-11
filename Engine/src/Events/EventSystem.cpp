#include "Events/EventSystem.h"

#include "Events/Event.h"

void EventSystem::Start(const SubsystemParams& params)
{
}

void EventSystem::Shutdown()
{
	m_listenerSystems.clear();
}

void EventSystem::AddListener(Subsystem& subsystem)
{
	m_listenerSystems.push_back(&subsystem);
}

void EventSystem::AddListener(Subsystem* subsystem)
{
	m_listenerSystems.push_back(subsystem);
}

void EventSystem::RemoveListener(Subsystem& subsystem)
{
	m_listenerSystems.erase(std::find(m_listenerSystems.begin(), m_listenerSystems.end(), &subsystem));
}

void EventSystem::DispatchEvent(Event event)
{
	for (auto listener : m_listenerSystems)
	{
		listener->OnEvent(event);
		if (event.Handled) break;
	}
}
