#pragma once

#include "Core/Subsystem.h"

struct GLFWwindow;
class EventSystem;

struct WindowProps : public SubsystemParams
{
	const char* title = "Solace";
	int width = 1280;
	int height = 720;
	EventSystem* eventSystem = nullptr;
};

class Window : public SingletonSubsystem<Window>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void PreAppUpdate() override;
	void PostAppUpdate() override;

	bool IsOpen();
private:
	GLFWwindow* m_GLFWInstance;
	EventSystem* m_EventSystem;
	const char* m_Title;
};