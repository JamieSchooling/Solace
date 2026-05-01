#pragma once

#include <entt/entt.hpp>

#include "Rendering/FrameRenderData.h"

class Scene
{
public:
	Scene();

	entt::registry Registry;
	std::string Name = "Scene";
private:
	entt::entity m_MainCamera;

	friend class SceneSystem;
};