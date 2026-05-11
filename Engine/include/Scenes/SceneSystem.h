#pragma once

#include "Core/Subsystem.h"
#include "Events/EventSystem.h"
#include "Scenes/Scene.h"

struct SceneSystemProps : public SubsystemParams
{
	EventSystem* EventSystem = nullptr;
};

class SceneSystem : public SingletonSubsystem<SceneSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;
	void OnAppUpdate() override;

	void OnEvent(Event& event) override;

	FrameRenderData& GetRenderData() { return m_frameRenderData; }
	Scene& GetActiveScene() { return m_activeScene; }
	void LoadScene(Scene& scene)
	{
		m_activeScene.Registry.swap(scene.Registry);
		m_activeScene.Name = scene.Name;
		m_activeScene.MainCamera = scene.MainCamera;
		Event e;
		e.Type = EventType::SceneLoad;
		m_eventSystem->DispatchEvent(e);
	}
private:
	Scene m_activeScene;
	EventSystem* m_eventSystem;

	FrameRenderData m_frameRenderData;
};