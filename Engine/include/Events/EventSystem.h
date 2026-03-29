#pragma once

#include <vector>

#include "Core/Subsystem.h"

struct Event;

class EventSystem : public SingletonSubsystem<EventSystem>
{
public:
	void Start(SubsystemParams params) override;
	void Shutdown() override;

	void AddListener(Subsystem& subsystem);
	void RemoveListener(Subsystem& subsystem);

	void DispatchEvent(Event event);

	// TODO: Implement event queue for deferred events (e.g. End of frame, Next frame, in 0.5 secs) <- No idea how to schedule like that yet though
private:
	std::vector<Subsystem*> m_ListenerSystems;
};