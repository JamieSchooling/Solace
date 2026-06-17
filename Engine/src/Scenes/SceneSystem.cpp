#include "Scenes/SceneSystem.h"

#include "Rendering/Camera.h"
#include "Rendering/Light.h"
#include "Rendering/MeshRender.h"
#include "Rendering/Material.h"
#include "Transform/Transform.h"
#include <Scenes/SceneSerialiser.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void SceneSystem::Start(const SubsystemParams& params)
{
	const SceneSystemProps& props = static_cast<const SceneSystemProps&>(params);

	m_eventSystem = props.EventSystem;
	m_eventSystem->AddListener(this);

	std::shared_ptr<Shader> shader = std::make_shared<Shader>("./resources/shaders/shadowVert.glsl", "./resources/shaders/shadowFrag.glsl");
	m_shadowPassMaterial = std::make_shared<Material>(shader);
	m_shadowView.RenderTarget = std::make_shared<FBO>(glm::ivec2(4096, 4096), std::vector<FBOAttachment>{ {AttachmentType::Depth} });

	Scene scene = Scene::CreateDefault();
	if (!props.StartupScene.is_nil() && AssetRegistry::Get().Exists(props.StartupScene))
	{
		std::filesystem::path scenePath = AssetRegistry::Get().GetFullPath(props.StartupScene);
		SceneSerialiser s(scene);
		s.DeserialiseFrom(scenePath);
	}
	LoadScene(scene);
}

void SceneSystem::Shutdown()
{
}

void SceneSystem::OnAppUpdate()
{
	m_renderQueue.clear();
	m_shadowQueue.clear();

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

			m_shadowView.Camera.Position = -40.0f * m_lightData.DLight.Direction;

			float orthoSize = 40.0f * 0.75f;
			m_shadowView.Camera.Projection = glm::ortho(
				-orthoSize, orthoSize,
				-orthoSize, orthoSize,
				0.1f, 100.f
			);
			m_shadowView.Camera.View = glm::lookAt(glm::vec3(m_shadowView.Camera.Position), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
		if (render.Material.expired())
		{
			render.ReloadMaterial();
		}
		if (auto mesh = render.Mesh.lock())
		{
			m_shadowQueue.emplace_back(mesh->DepthGeometry, m_shadowPassMaterial, transform.GetTransformMatrix());
			m_renderQueue.emplace_back(mesh->Geometry, render.Material.lock(), transform.GetTransformMatrix());
		}
	}
}

void SceneSystem::LoadScene(Scene& scene)
{
	m_activeScene.Registry.swap(scene.Registry);
	m_activeScene.Name = scene.Name;
	m_activeScene.MainCamera = scene.MainCamera;

	for (auto& entity : m_activeScene.Registry.view<entt::entity>())
	{
		auto componentReflections = ReflectionRegistry::View(m_activeScene.Registry, entity);
		for (auto& component : componentReflections)
		{
			component->Initialise(m_activeScene.Registry, entity);
		}
	}

	Event e;
	e.Type = EventType::SceneLoad;
	m_eventSystem->DispatchEvent(e);
}
