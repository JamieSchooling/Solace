#include "Scenes/Scene.h"

#include "Rendering/Camera.h"
#include "Transform/Transform.h"

Scene::Scene()
{
	m_MainCamera = Registry.create();
	Registry.emplace<Camera>(m_MainCamera);
	Registry.emplace<Transform>(m_MainCamera);
}
