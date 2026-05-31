#pragma once

#include "Core/Subsystem.h"

#include <glm/glm.hpp>
#include "Input/KeyCodes.h"

#include <string>

#include "Events/EventSystem.h"
#include <variant>

enum class InputPhase
{
	Released = 0,
	Pressed = 1,
	Held = 2,

	None = -1
};

using InputData = std::variant<std::monostate, float, glm::vec2>;

struct InputAction
{
	std::string name;
	std::vector<InputBinding> keys;
	std::function<void(InputAction&)> computeEvent;
	InputData data;

	template<typename T>
	T Get() const
	{
		return std::get<T>(data);
	}
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

	void PreAppUpdate() override;
	void FinaliseAppUpdate() override;
	void OnEvent(Event& event) override;

	void AddAction(std::string action, InputBinding key);
	void AddTwoComponentAction(std::string action, InputBinding leftKey, InputBinding rightKey);
	void AddFourComponentAction(std::string action, InputBinding upKey, InputBinding leftKey, InputBinding downKey, InputBinding rightKey, bool normalise = true);

	bool WasKeyPressedThisFrame(InputBinding key) const;
	bool WasKeyReleasedThisFrame(InputBinding key) const;
	bool IsKeyDown(InputBinding key) const;

	bool WasKeyPressedThisFrame(std::string action) const;
	bool WasKeyReleasedThisFrame(std::string action) const;
	bool IsKeyDown(std::string action) const;
	const InputAction* GetAction(std::string action) const;

	glm::vec2 MousePos() const;
	glm::vec2 MouseDelta() const;
private:
	EventSystem* m_eventSystem;
	std::unordered_map<InputBinding, InputPhase> m_keyStates;
	std::unordered_map<InputBinding, InputPhase> m_lastFrameKeyStates;
	std::unordered_map<InputBinding, std::vector<std::string>> m_actionBindings;
	std::unordered_map<std::string, InputAction> m_inputActions;
	glm::vec2 m_lastMousePos;
	glm::vec2 m_mouseDelta = glm::vec2(-1.0f);

	std::vector<Event> m_inputEvents;

	void HandleKeyEvent(int key, int action);
	void HandleMouseButtonEvent(int button, int action);
	void HandleMouseMoveEvent(double mouseX, double mouseY);
};