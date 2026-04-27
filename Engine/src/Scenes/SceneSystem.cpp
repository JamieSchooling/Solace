#include "Scenes/SceneSystem.h"

#include "Rendering/Camera.h"
#include "Transform/Transform.h"
#include "Rendering/MeshRender.h"

#include "Scenes/DefaultScene.h"

void SceneSystem::Start(const SubsystemParams& params)
{
	const SceneSystemProps& props = static_cast<const SceneSystemProps&>(params);

	props.eventSystem->AddListener(this);

	m_ActiveScene = BuildDefaultScene();
}

void SceneSystem::Shutdown()
{
}

void SceneSystem::OnAppUpdate()
{
	m_FrameRenderData.renderQueue.clear();

	Camera& cam = m_ActiveScene.Registry.get<Camera>(m_ActiveScene.m_MainCamera);
	Transform& camTransform = m_ActiveScene.Registry.get<Transform>(m_ActiveScene.m_MainCamera);

	m_FrameRenderData.cameraProjection = cam.GetProjection();
	m_FrameRenderData.cameraView = glm::inverse(camTransform.GetTransformMatrix());

	const auto& view = m_ActiveScene.Registry.view<MeshRenderComponent, Transform>();

	for (auto [entity, render, transform] : view.each()) 
	{
		m_FrameRenderData.renderQueue.emplace_back(render.Geometry, render.Material, transform.GetTransformMatrix());
	}
}

void SceneSystem::OnEvent(Event& event)
{
	if (event.type == EventType::WindowResize)
	{
		m_ActiveScene.Registry.get<Camera>(m_ActiveScene.m_MainCamera).RecalculateProjection();
	}
}
