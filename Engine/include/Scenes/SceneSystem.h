#pragma once

#include "Core/Subsystem.h"
#include "Events/EventSystem.h"
#include "Scenes/Scene.h"

struct SceneSystemProps : public SubsystemParams
{
	EventSystem* eventSystem = nullptr;
};

class SceneSystem : public SingletonSubsystem<SceneSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;
	void OnAppUpdate() override;

	void OnEvent(Event& event) override;

	FrameRenderData& GetRenderData() { return m_FrameRenderData; }
	Scene& GetActiveScene() { return m_ActiveScene; }
	void LoadScene(Scene& scene)
	{
		m_ActiveScene.Registry.swap(scene.Registry);
		m_ActiveScene.Name = scene.Name;
		m_ActiveScene.MainCamera = scene.MainCamera;
		Event e;
		e.type = EventType::SceneLoad;
		m_EventSystem->DispatchEvent(e);
	}
private:
	Scene m_ActiveScene;
	EventSystem* m_EventSystem;

	FrameRenderData m_FrameRenderData;
};