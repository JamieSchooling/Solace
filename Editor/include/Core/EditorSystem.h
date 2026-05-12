#pragma once

#include <Core/Subsystem.h>
#include <Events/EventSystem.h>
#include <Scenes/SceneSystem.h>

#include <GLFW/glfw3.h>
#include <memory>
#include <Rendering/FBO.h>

#include "Inspectors/ComponentInspector.h"

#include <entt/entt.hpp>

#include "Windows/EditorWindow.h"
#include "Menu/MenuRegistry.h"

class Scene;

enum class LayoutOption
{
	Default,
	Active
};

struct EditorSystemProps : public SubsystemParams
{
	GLFWwindow* GLFWInstance;
	EventSystem* EventSystem;
	// For later use when viewport windows are implemented
	//std::shared_ptr<FBO> GameRenderTarget;
};

class EditorSystem : public SingletonSubsystem<EditorSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void PreAppUpdate() override;
	void OnAppUpdate() override;
	void OnEvent(Event& e) override;

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<EditorWindow, T>>>
	inline void OpenWindow()
	{
		auto it = std::find_if(m_windows.begin(), m_windows.end(), [](const std::unique_ptr<EditorWindow>& window)
		{
			return dynamic_cast<T*>(window.get()) != nullptr;
		});

		bool instanceExists = it != m_windows.end();
		if (!instanceExists)
		{
			m_windows.push_back(std::make_unique<T>());
			m_windows.back()->Initialise(SceneSystem::Get().GetActiveScene());
		}
	}

	LayoutOption GetLayout() const { return m_currentLayout; }
	void SetLayout(LayoutOption layout) { m_newLayout = layout; }
private:
	std::shared_ptr<FBO> m_gameRenderTarget;
	LayoutOption m_currentLayout = LayoutOption::Default;
	LayoutOption m_newLayout = LayoutOption::Default;
	entt::entity m_selectedEntity = entt::null;
	std::vector<std::unique_ptr<EditorWindow>> m_windows;

	void HandleLayoutChange();
	void DrawMenuBar();
	void DrawMenuNode(MenuNode& node);
};
