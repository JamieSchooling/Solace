#pragma once

#include <utility>

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

	std::vector<RenderItem>& FetchVisibleObjectData();
	std::vector<std::pair<Camera, Transform>>& FetchActiveCameras();
	RenderView& GetShadowView() { return m_shadowView; }
	LightData& FetchLightData();
	Scene& GetActiveScene() { return m_activeScene; }
	void LoadScene(Scene& scene);
	
private:
	Scene m_activeScene;
	EventSystem* m_eventSystem;

	std::vector<RenderItem> m_visibleObjectData;
	std::vector<std::pair<Camera, Transform>> m_activeCameras;
	RenderView m_shadowView;
	std::shared_ptr<Material> m_shadowPassMaterial;
	LightData m_lightData;
};