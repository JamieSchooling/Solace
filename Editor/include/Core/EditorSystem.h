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
	void PostAppUpdate() override;
	void OnEvent(Event& e) override;

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<EditorWindow, T>>>
	inline T& OpenWindow()
	{
		auto it = std::find_if(m_windows.begin(), m_windows.end(), 
			[](const std::unique_ptr<EditorWindow>& window)
			{
				return dynamic_cast<T*>(window.get()) != nullptr;
			}
		);

		bool instanceExists = it != m_windows.end();
		if (instanceExists)
		{
			return *dynamic_cast<T*>(it->get());
		}

		auto window = std::make_unique<T>();
		T& windowRef = *window;
		m_windows.push_back(std::move(window));
		m_windows.back()->Initialise(SceneSystem::Get().GetActiveScene());
		m_windows.back()->Initialise();
		return windowRef;
	}

	LayoutOption GetLayout() const { return m_currentLayout; }
	void SetLayout(LayoutOption layout) { m_newLayout = layout; }

	CameraData GetEditorCameraData() 
	{
		CameraData data;
		data.Projection = m_editorCamera.GetProjection();
		data.View = m_editorCamera.GetView(m_editorCamTransform);
		data.Position = glm::vec4(m_editorCamTransform.Position, 0.0f);

		return data;
	}
private:
	std::shared_ptr<FBO> m_gameRenderTarget;
	LayoutOption m_currentLayout = LayoutOption::Default;
	LayoutOption m_newLayout = LayoutOption::Default;
	entt::entity m_selectedEntity = entt::null;
	std::vector<std::unique_ptr<EditorWindow>> m_windows;

	Camera m_editorCamera;
	Transform m_editorCamTransform;
	glm::vec3 m_editorCamRotation;
	float m_editorFlyCamSpeed = 2.5f;

	void HandleLayoutChange();
	void DrawMenuBar();
	void DrawMenuNode(MenuNode& node);
};
