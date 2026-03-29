#pragma once

#include "Core/Subsystem.h"

struct GLFWwindow;

struct WindowProps : public SubsystemParams
{
	const char* title = "Solace";
	int width = 1280;
	int height = 720;
};

class Window : public SingletonSubsystem<Window>
{
public:
	void Start(SubsystemParams params = WindowProps{}) override;
	void Shutdown() override;

	void PreAppUpdate() override;
	void PostAppUpdate() override;

	bool IsOpen();
private:
	GLFWwindow* m_GLFWInstance;
	const char* m_Title;
};