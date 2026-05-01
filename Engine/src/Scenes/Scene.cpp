#include "Scenes/Scene.h"

#include "Rendering/Camera.h"
#include "Transform/Transform.h"
#include "Scenes/NameComponent.h"

Scene::Scene()
{
	MainCamera = Registry.create();
	Registry.emplace<NameComponent>(MainCamera).Name = "Main Camera";
	Registry.emplace<Transform>(MainCamera);
	Registry.emplace<Camera>(MainCamera);
}
