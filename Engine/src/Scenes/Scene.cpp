#include "Scenes/Scene.h"

#include "Rendering/Camera.h"
#include "Rendering/Light.h"
#include "Transform/Transform.h"
#include "Scenes/NameComponent.h"
#include <Rendering/MeshRender.h>
#include <Core/Application.h>

Scene::Scene()
{
	
}

Scene Scene::CreateDefault()
{
	Scene scene;
	scene.Name = "Default Scene";

	entt::entity mainCamera = scene.Registry.create();
	scene.Registry.emplace<NameComponent>(mainCamera).Name = "Main Camera";
	scene.Registry.emplace<Transform>(mainCamera);
	scene.Registry.emplace<Camera>(mainCamera);
	scene.MainCamera = mainCamera;

	entt::entity directionalLight = scene.Registry.create();
	scene.Registry.emplace<NameComponent>(directionalLight).Name = "Directional Light";
	scene.Registry.emplace<Transform>(directionalLight).Rotation = glm::quat(glm::radians(glm::vec3(-15.0f, 20.0f, 0.0f)));
	scene.Registry.emplace<Light>(directionalLight);

	entt::entity cube = scene.Registry.create();

	scene.Registry.emplace<NameComponent>(cube).Name = "Cube";

	MeshRenderComponent& meshRender = scene.Registry.emplace<MeshRenderComponent>(cube);
	meshRender.Mesh = AssetRegistry::Get().RegisterNewAsset(Application::GetResourcePath() / "meshes" / "cube.obj");
	//meshRender.Mesh = (Application::GetResourcePath() / "meshes" / "cube.obj").string();
	
	scene.Registry.emplace<Transform>(cube);

	return scene;
}
