#pragma once

#include "Events/Event.h"

struct SubsystemParams {};

class Subsystem
{
public:
	virtual void Start(SubsystemParams params) = 0;
	virtual void Shutdown() = 0;

	virtual void PreAppUpdate() {}
	virtual void OnAppUpdate() {}
	virtual void PostAppUpdate() {}

	virtual void OnEvent(Event& event) {}
protected:
	Subsystem() = default;
	~Subsystem() = default;
};

template<typename Derived>
class SingletonSubsystem : public Subsystem
{
public:
	static Derived& Get()
	{
		static Derived sInstance;
		return sInstance;
	}
protected:
	SingletonSubsystem() = default;
	~SingletonSubsystem() = default;
};