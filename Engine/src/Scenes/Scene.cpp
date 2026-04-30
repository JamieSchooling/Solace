#include "Scenes/Scene.h"

#include "Rendering/Camera.h"
#include "Transform/Transform.h"
#include "Scenes/NameComponent.h"

Scene::Scene()
{
	m_MainCamera = Registry.create();
	Registry.emplace<NameComponent>(m_MainCamera).Name = "Main Camera";
	Registry.emplace<Transform>(m_MainCamera);
	Registry.emplace<Camera>(m_MainCamera);
}
