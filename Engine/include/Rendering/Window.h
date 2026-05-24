#pragma once

#include "Core/Subsystem.h"

struct GLFWwindow;
class EventSystem;

struct WindowProps : public SubsystemParams
{
	const char* Title = "Solace";
	int Width = 1280;
	int Height = 720;
	EventSystem* EventSystem = nullptr;
};

enum class CursorMode
{
	Visible,
	Hidden,
	Disabled
};

class Window : public SingletonSubsystem<Window>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void PreAppUpdate() override;
	void FinaliseAppUpdate() override;

	bool IsOpen();

	inline float GetWidth() const { return m_width; }
	inline float GetHeight() const { return m_height; }
	inline float GetAspectRatio() const { return m_aspectRatio; }
	inline float CurrentTime() const { return m_currentTime; }
	inline float DeltaTime() const { return m_deltaTime; }

	void SetCursorMode(CursorMode mode);

	inline GLFWwindow* GetGLFWInstance() { return m_glfwInstance; }
private:
	GLFWwindow* m_glfwInstance;
	EventSystem* m_eventSystem;
	const char* m_title;
	float m_width;
	float m_height;
	float m_aspectRatio;
	float m_currentTime;
	float m_prevFrameTime;
	float m_deltaTime;
};