#include "Scenes/Scene.h"

#include "Rendering/Camera.h"
#include "Rendering/Light.h"
#include "Transform/Transform.h"
#include "Scenes/NameComponent.h"

Scene::Scene()
{
	MainCamera = Registry.create();
	Registry.emplace<NameComponent>(MainCamera).Name = "Main Camera";
	Registry.emplace<Transform>(MainCamera);
	Registry.emplace<Camera>(MainCamera);

	auto directionalLight = Registry.create();
	Registry.emplace<NameComponent>(directionalLight).Name = "Directional Light";
	Registry.emplace<Transform>(directionalLight);
	Registry.emplace<Light>(directionalLight);
}
