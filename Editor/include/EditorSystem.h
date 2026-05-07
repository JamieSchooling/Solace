#pragma once

#include <Core/Subsystem.h>
#include <Events/EventSystem.h>

#include <GLFW/glfw3.h>
#include <memory>
#include <Rendering/FBO.h>

#include "ComponentInspector.h"

#include <entt/entt.hpp>

class Scene;

enum class LayoutOption
{
	Default,
	Active
};

struct EditorSystemProps : public SubsystemParams
{
	GLFWwindow* GLFWInstance;
	EventSystem* eventSystem;
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

private:
	std::shared_ptr<FBO> m_GameRenderTarget;
	LayoutOption m_CurrentLayout = LayoutOption::Default;
	LayoutOption m_NewLayout = LayoutOption::Default;
	entt::entity m_SelectedEntity = entt::null;
	std::unordered_map<entt::entity, std::vector<std::shared_ptr<ComponentInspector>>> m_Inspectors;

	void ConstructInspectors();

	void HandleLayoutChange();
	void DrawMenuBar();
	void DrawSceneHierarchy(Scene& scene);
	void DrawInspector(entt::registry& registry);
};