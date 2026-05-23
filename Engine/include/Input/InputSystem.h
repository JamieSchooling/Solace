#pragma once

#include "Core/Subsystem.h"

#include <glm/glm.hpp>
#include "Input/KeyCodes.h"

#include "Events/EventSystem.h"

enum class InputPhase
{
	Released = 0,
	Pressed = 1,
	Held = 2,

	Unknown = -1
};

struct InputSystemProps : public SubsystemParams
{
	EventSystem* EventSystem = nullptr;
};

class InputSystem : public SingletonSubsystem<InputSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void OnEvent(Event& event) override;

	bool WasKeyPressedThisFrame(InputBinding key) const;
	bool WasKeyReleasedThisFrame(InputBinding key) const;
	bool IsKeyDown(InputBinding key) const;
	glm::vec2 MousePos() const;
	glm::vec2 MouseDelta() const;
private:
	EventSystem* m_eventSystem;
	std::unordered_map<InputBinding, InputPhase> m_keyStates;
	glm::vec2 m_lastMousePos;
	glm::vec2 m_mouseDelta = glm::vec2(-1.0f);

	void HandleKeyEvent(int key, int action);
	void HandleMouseButtonEvent(int button, int action);
	void HandleMouseMoveEvent(double mouseX, double mouseY);
};