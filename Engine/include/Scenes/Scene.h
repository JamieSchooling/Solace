#pragma once

#include <entt/entt.hpp>

#include "Rendering/FrameRenderData.h"

class Scene
{
public:
	Scene();

	entt::registry Registry;
	std::string Name = "Scene";
	entt::entity MainCamera;
};