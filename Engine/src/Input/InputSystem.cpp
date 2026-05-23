#include "Input/InputSystem.h"

void InputSystem::Start(const SubsystemParams& params)
{
	const InputSystemProps& props = static_cast<const InputSystemProps&>(params);

	m_eventSystem = props.EventSystem;
	m_eventSystem->AddListener(this);
}

void InputSystem::Shutdown()
{
	m_keyStates.clear();
}

void InputSystem::OnEvent(Event& event)
{
	if (event.Type == EventType::InputKey)
	{
		HandleKeyEvent(event.InputKeyArgs.Key, event.InputKeyArgs.Action);
		event.Handled = true;
	}
	else if (event.Type == EventType::InputMouseButton)
	{
		HandleMouseButtonEvent(event.InputMouseButtonArgs.Button, event.InputMouseButtonArgs.Action);
		event.Handled = true;
	}
	else if (event.Type == EventType::InputMouseMove)
	{
		HandleMouseMoveEvent(event.InputMouseMoveArgs.MouseX, event.InputMouseMoveArgs.MouseY);
		event.Handled = true;
	}
}

bool InputSystem::WasKeyPressedThisFrame(InputBinding key) const
{
	if (!m_keyStates.contains(key)) { return false; }

	return m_keyStates.at(key) == InputPhase::Pressed;
}

bool InputSystem::WasKeyReleasedThisFrame(InputBinding key) const
{
	if (!m_keyStates.contains(key)) { return false; }

	return m_keyStates.at(key) == InputPhase::Released;
}

bool InputSystem::IsKeyDown(InputBinding key) const
{
	if (!m_keyStates.contains(key)) { return false; }

	return m_keyStates.at(key) == InputPhase::Held;
}

glm::vec2 InputSystem::MousePos() const
{
	return m_lastMousePos;
}

glm::vec2 InputSystem::MouseDelta() const
{
	return m_mouseDelta;
}

void InputSystem::HandleKeyEvent(int keycode, int action)
{
	InputBinding key = (InputBinding)keycode;
	InputPhase inputPhase = (InputPhase)action;

	m_keyStates[key] = inputPhase;
}

void InputSystem::HandleMouseButtonEvent(int buttonCode, int action)
{
	InputBinding button = (InputBinding)buttonCode;
	InputPhase inputPhase = (InputPhase)action;

	m_keyStates[button] = inputPhase;
}

void InputSystem::HandleMouseMoveEvent(double mouseX, double mouseY)
{
	glm::vec2 pos(mouseX, mouseY);
	if (m_lastMousePos != glm::vec2(-1.0f))
	{
		m_mouseDelta += pos - m_lastMousePos;
	}
	else
	{
		m_mouseDelta = glm::vec2(0.0f);
	}
	m_lastMousePos = pos;
}
