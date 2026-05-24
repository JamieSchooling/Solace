#include "Scenes/SceneSystem.h"

#include "Rendering/Camera.h"
#include "Rendering/Light.h"
#include "Rendering/MeshRender.h"
#include "Transform/Transform.h"

#include "Scenes/DefaultScene.h"

void SceneSystem::Start(const SubsystemParams& params)
{
	const SceneSystemProps& props = static_cast<const SceneSystemProps&>(params);

	m_eventSystem = props.EventSystem;
	m_eventSystem->AddListener(this);

	m_activeScene = BuildDefaultScene();
}

void SceneSystem::Shutdown()
{
}

void SceneSystem::OnAppUpdate()
{
	m_renderQueue.clear();

	if (m_activeScene.MainCamera == entt::null)
	{
		const auto& cameraView = m_activeScene.Registry.view<Camera>();
		for (auto entity : cameraView)
		{
			m_activeScene.MainCamera = entity;
		}
	}

	// Set light data
	m_lightData = LightData();

	const auto& lightView = m_activeScene.Registry.view<Light, Transform>();
	int pLightCount = 0;
	for (auto [entity, light, transform] : lightView.each())
	{
		if (light.Type == LightType::Directional)
		{
			m_lightData.DLight.Colour = light.Colour.ColourValue;
			m_lightData.DLight.Direction = glm::vec4(transform.Forward(), 0.0f);
			m_lightData.DLight.Intensity = light.Intensity;
		}
		if (light.Type == LightType::Point)
		{
			m_lightData.PLights[pLightCount].Colour = light.Colour.ColourValue;
			m_lightData.PLights[pLightCount].Position = glm::vec4(transform.Position, 0.0f);
			m_lightData.PLights[pLightCount].Radius = light.Radius;
			m_lightData.PLights[pLightCount].Intensity = light.Intensity;
			pLightCount++;
		}
	}
	m_lightData.NumPointLights = pLightCount;

	const auto& view = m_activeScene.Registry.view<MeshRenderComponent, Transform>();

	for (auto [entity, render, transform] : view.each()) 
	{
		m_renderQueue.emplace_back(render.Geometry, render.Material, transform.GetTransformMatrix());
	}
}

void SceneSystem::OnEvent(Event& event)
{
	if (event.Type == EventType::WindowResize)
	{
		if (event.WindowResizeArgs.Width > 0 && event.WindowResizeArgs.Height > 0)
		{
			m_activeScene.Registry.get<Camera>(m_activeScene.MainCamera).RecalculateProjection();
		}
	}
}
