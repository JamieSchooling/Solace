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

	inline float GetWidth() const { return m_Width; }
	inline float GetHeight() const { return m_Height; }
	inline float GetAspectRatio() const { return m_AspectRatio; }

	inline GLFWwindow* GetGLFWInstance() { return m_GLFWInstance; }
private:
	GLFWwindow* m_GLFWInstance;
	EventSystem* m_EventSystem;
	const char* m_Title;
	float m_Width;
	float m_Height;
	float m_AspectRatio;
};