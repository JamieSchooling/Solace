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
	m_frameRenderData.RenderQueue.clear();

	Camera& cam = m_activeScene.Registry.get<Camera>(m_activeScene.MainCamera);
	Transform& camTransform = m_activeScene.Registry.get<Transform>(m_activeScene.MainCamera);

	// Set camera data
	m_frameRenderData.Camera.Projection = cam.GetProjection();
	m_frameRenderData.Camera.View = glm::inverse(camTransform.GetTransformMatrix());
	m_frameRenderData.Camera.Position = glm::vec4(camTransform.Position, 0.0f);

	// Set light data
	auto lightView = m_activeScene.Registry.view<Light>();
	if (lightView.size() > 0)
	{
		auto lightEntity = lightView.front();
		Light& light = m_activeScene.Registry.get<Light>(lightEntity);
		Transform& lightTransform = m_activeScene.Registry.get<Transform>(lightEntity);
		m_frameRenderData.Lights.DLight.Colour = light.Colour.ColourValue;
		m_frameRenderData.Lights.DLight.Direction = glm::vec4(lightTransform.Forward(), 0.0f);
	}

	const auto& view = m_activeScene.Registry.view<MeshRenderComponent, Transform>();

	for (auto [entity, render, transform] : view.each()) 
	{
		m_frameRenderData.RenderQueue.emplace_back(render.Geometry, render.Material, transform.GetTransformMatrix());
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
