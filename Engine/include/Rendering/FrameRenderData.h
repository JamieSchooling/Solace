#pragma once

#include "Rendering/VAO.h"
#include "Rendering/Material.h"

struct RenderItem
{
	std::shared_ptr<VAO> geometry;
	std::shared_ptr<Material> material;
	glm::mat4 transform = glm::mat4(1.0f);
};

using RenderQueue = std::vector<RenderItem>;

struct FrameRenderData
{
	RenderQueue renderQueue;
	glm::mat4 cameraView = glm::mat4(1.0f);
	glm::mat4 cameraProjection = glm::mat4(1.0f);
};