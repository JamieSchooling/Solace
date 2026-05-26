#pragma once

#include <entt/entt.hpp>

#include "Rendering/FrameRenderData.h"
#include <Rendering/Camera.h>

class Scene
{
public:
	Scene();

	entt::registry Registry;
	std::string Name = "Scene";
	entt::entity MainCamera = entt::null;

	CameraData GetMainCameraData()
	{
		Camera& cam = Registry.get<Camera>(MainCamera);
		Transform& camTransform = Registry.get<Transform>(MainCamera);
		CameraData data;
		data.Projection = cam.GetProjection();
		data.View = cam.GetView(camTransform);
		data.Position = glm::vec4(camTransform.Position, 0.0f);

		return data;
	}

	static Scene CreateDefault();
};