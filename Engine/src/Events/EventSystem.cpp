#include "Events/EventSystem.h"

#include "Events/Event.h"

void EventSystem::Start(const SubsystemParams& params)
{
}

void EventSystem::Shutdown()
{
	m_ListenerSystems.clear();
}

void EventSystem::AddListener(Subsystem& subsystem)
{
	m_ListenerSystems.push_back(&subsystem);
}

void EventSystem::AddListener(Subsystem* subsystem)
{
	m_ListenerSystems.push_back(subsystem);
}

void EventSystem::RemoveListener(Subsystem& subsystem)
{
	m_ListenerSystems.erase(std::find(m_ListenerSystems.begin(), m_ListenerSystems.end(), &subsystem));
}

void EventSystem::DispatchEvent(Event event)
{
	for (auto listener : m_ListenerSystems)
	{
		listener->OnEvent(event);
		if (event.handled) break;
	}
}
