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
	std::shared_ptr<FBO> EditorRenderTarget;
	std::shared_ptr<FBO> GameRenderTarget;
};

class EditorSystem : public SingletonSubsystem<EditorSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void PreAppUpdate() override;
	void PostAppUpdate() override;
	void FinaliseAppUpdate() override;
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
			T& instance = *dynamic_cast<T*>(it->get());
			ImGui::SetWindowFocus(instance.Title());
			return instance;
		}

		auto window = std::make_unique<T>();
		T& windowRef = *window;
		ImGui::SetWindowFocus(windowRef.Title());
		m_windows.push_back(std::move(window));
		m_windows.back()->Initialise(SceneSystem::Get().GetActiveScene());
		m_windows.back()->Initialise();
		return windowRef;
	}

	LayoutOption GetLayout() const { return m_currentLayout; }
	void SetLayout(LayoutOption layout) { m_newLayout = layout; }

	std::filesystem::path GetCurrentlyOpenScene() const;
	void SetCurrentlyOpenScene(std::filesystem::path path);
	void SetSceneDirty(bool dirty = true);
	bool IsSceneDirty() const;

	void ShowSceneSaveModal();

	CameraData GetEditorCameraData() 
	{
		CameraData data;
		data.Projection = m_editorCamera.GetProjection();
		data.View = m_editorCamera.GetView(m_editorCamTransform);
		data.Position = glm::vec4(m_editorCamTransform.Position, 0.0f);

		return data;
	}

	void RecalcCamProjection();

	std::shared_ptr<FBO> GetSceneRenderTarget();
	std::shared_ptr<FBO> GetGameRenderTarget();
	Transform& GetEditorCamTransform();
private:
	std::shared_ptr<FBO> m_gameRenderTarget;
	std::shared_ptr<FBO> m_editorRenderTarget;
	LayoutOption m_currentLayout = LayoutOption::Default;
	LayoutOption m_newLayout = LayoutOption::Default;
	entt::entity m_selectedEntity = entt::null;
	std::vector<std::unique_ptr<EditorWindow>> m_windows;

	std::unique_ptr<Texture> m_playIcon;
	std::unique_ptr<Texture> m_stopIcon;

	std::filesystem::path m_currentlyOpenScene;

	Camera m_editorCamera;
	Transform m_editorCamTransform;

	bool m_sceneDirty = false;
	bool m_showSceneSaveModal = false;

	void DrawSceneSaveModal();
	void HandleLayoutChange();
	void DrawMenuBar();
	void DrawMenuNode(MenuNode& node);
	void DrawToolbar();
};
