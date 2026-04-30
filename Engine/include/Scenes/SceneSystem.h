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
private:
	Scene m_ActiveScene;

	FrameRenderData m_FrameRenderData;
};