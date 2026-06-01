#pragma once

#include "Core/Subsystem.h"
#include "Events/EventSystem.h"
#include "Scenes/Scene.h"

struct SceneSystemProps : public SubsystemParams
{
	EventSystem* EventSystem = nullptr;
	AssetHandle StartupScene;
};

class SceneSystem : public SingletonSubsystem<SceneSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;
	void OnAppUpdate() override;

	void OnEvent(Event& event) override;

	std::vector<RenderItem>& GetRenderQueue() { return m_renderQueue; }
	std::vector<RenderItem>& GetShadowQueue() { return m_shadowQueue; }
	RenderView& GetShadowView() { return m_shadowView; }
	LightData& GetLightData() { return m_lightData; }
	Scene& GetActiveScene() { return m_activeScene; }
	void LoadScene(Scene& scene);
	
private:
	Scene m_activeScene;
	EventSystem* m_eventSystem;

	std::vector<RenderItem> m_renderQueue;
	std::vector<RenderItem> m_shadowQueue;
	RenderView m_shadowView;
	std::shared_ptr<Material> m_shadowPassMaterial;
	LightData m_lightData;
};