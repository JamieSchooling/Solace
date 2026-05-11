#include "Scenes/SceneSystem.h"

#include "Rendering/Camera.h"
#include "Transform/Transform.h"
#include "Rendering/MeshRender.h"

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

	m_frameRenderData.CameraProjection = cam.GetProjection();
	m_frameRenderData.CameraView = glm::inverse(camTransform.GetTransformMatrix());

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
