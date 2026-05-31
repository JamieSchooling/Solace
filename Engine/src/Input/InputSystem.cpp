#include "Input/InputSystem.h"
#include <iostream>

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

void InputSystem::PreAppUpdate()
{
	m_mouseDelta = glm::vec2(0.0f);

	for (auto& [key, phase] : m_keyStates)
	{		
		if (phase != InputPhase::Held)
		{
			phase = InputPhase::None;
		}
	}

	for (const auto& event : m_inputEvents)
	{
		if (event.Type == EventType::InputKey)
		{
			HandleKeyEvent(event.InputKeyArgs.Key, event.InputKeyArgs.Action);
		}
		else if (event.Type == EventType::InputMouseButton)
		{
			HandleMouseButtonEvent(event.InputMouseButtonArgs.Button, event.InputMouseButtonArgs.Action);
		}
		else if (event.Type == EventType::InputMouseMove)
		{
			HandleMouseMoveEvent(event.InputMouseMoveArgs.MouseX, event.InputMouseMoveArgs.MouseY);
		}
	}
	m_inputEvents.clear();

	for (auto& [key, phase] : m_keyStates)
	{
		if (phase == InputPhase::None && m_lastFrameKeyStates[key] == InputPhase::Pressed)
		{
			phase = InputPhase::Held;
		}
	}
}

void InputSystem::FinaliseAppUpdate()
{
	m_lastFrameKeyStates = m_keyStates;
}

void InputSystem::OnEvent(Event& event)
{
	switch (event.Type)
	{
	case EventType::InputKey:
	case EventType::InputMouseButton:
	case EventType::InputMouseMove:
		m_inputEvents.push_back(event);
		event.Handled = true;
	}
}

void InputSystem::AddAction(std::string action, InputBinding key)
{
	m_actionBindings[key].push_back(action);
	m_inputActions[action] = InputAction{
		action,
		{ key },
		nullptr
	};
}

void InputSystem::AddTwoComponentAction(std::string action, InputBinding leftKey, InputBinding rightKey)
{
	m_actionBindings[leftKey].push_back(action);
	m_actionBindings[rightKey].push_back(action);
	m_inputActions[action] = InputAction{
		action,
		{ leftKey, rightKey },
		[=](InputAction& action) 
		{
			float direction = 0;
			if (IsKeyDown(leftKey)) direction--;
			if (IsKeyDown(rightKey)) direction++;

			action.data = direction;
		},
		0.0f
	};
}

void InputSystem::AddFourComponentAction(std::string action, InputBinding upKey, InputBinding leftKey, InputBinding downKey, InputBinding rightKey, bool normalise)
{
	m_actionBindings[upKey].push_back(action);
	m_actionBindings[leftKey].push_back(action);
	m_actionBindings[downKey].push_back(action);
	m_actionBindings[rightKey].push_back(action);
	m_inputActions[action] = InputAction{
		action,
		{ upKey, leftKey, downKey, rightKey },
		[=](InputAction& action)
		{
			glm::vec2 vector(0.0f);
			if (IsKeyDown(upKey)) vector.y++;
			if (IsKeyDown(leftKey)) vector.x--;
			if (IsKeyDown(downKey)) vector.y--;
			if (IsKeyDown(rightKey)) vector.x++;
			if (normalise && vector != glm::vec2(0.0f)) vector = glm::normalize(vector);
			action.data = vector;
		},
		glm::vec2(0.0f)
	};
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

	return m_keyStates.at(key) == InputPhase::Pressed || m_keyStates.at(key) == InputPhase::Held;
}

bool InputSystem::WasKeyPressedThisFrame(std::string action) const
{
	if (!m_inputActions.contains(action)) { return false; }

	for (auto key : m_inputActions.at(action).keys)
	{
		if (WasKeyPressedThisFrame(key)) { return true; }
	}
	return false;
}

bool InputSystem::WasKeyReleasedThisFrame(std::string action) const
{
	if (!m_inputActions.contains(action)) { return false; }

	for (auto key : m_inputActions.at(action).keys)
	{
		if (WasKeyReleasedThisFrame(key)) { return true; }
	}
	return false;
}

bool InputSystem::IsKeyDown(std::string action) const
{
	if (!m_inputActions.contains(action)) { return false; }

	for (auto key : m_inputActions.at(action).keys)
	{
		if (IsKeyDown(key)) { return true; }
	}
	return false;
}

const InputAction* InputSystem::GetAction(std::string action) const
{
	if (!m_inputActions.contains(action)) { return nullptr; }

	return &m_inputActions.at(action);
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

	if (m_actionBindings.contains(key))
	{
		for (auto actionName : m_actionBindings.at(key))
		{
			InputAction& action = m_inputActions.at(actionName);
			if (action.computeEvent != nullptr)
				action.computeEvent(action);
		}
	}
}

void InputSystem::HandleMouseButtonEvent(int buttonCode, int action)
{
	InputBinding button = (InputBinding)buttonCode;
	InputPhase inputPhase = (InputPhase)action;
	m_keyStates[button] = inputPhase;

	if (m_actionBindings.contains(button))
	{
		for (auto& actionName : m_actionBindings.at(button))
		{
			InputAction& action = m_inputActions.at(actionName);
			if (action.computeEvent != nullptr)
				action.computeEvent(action);
		}
	}
}

void InputSystem::HandleMouseMoveEvent(double mouseX, double mouseY)
{
	glm::vec2 pos(mouseX, mouseY);
	if (m_lastMousePos == glm::vec2(-1.0f))
	{
		m_mouseDelta = glm::vec2(0.0f);
	}
	else
	{
		m_mouseDelta += pos - m_lastMousePos;
	}
	m_lastMousePos = pos;
}
