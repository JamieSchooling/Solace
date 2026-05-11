#pragma once

#include "Rendering/VAO.h"
#include "Rendering/Material.h"

struct RenderItem
{
	std::shared_ptr<VAO> Geometry;
	std::shared_ptr<Material> Material;
	glm::mat4 Transform = glm::mat4(1.0f);
};

using RenderQueue = std::vector<RenderItem>;

struct FrameRenderData
{
	RenderQueue RenderQueue;
	glm::mat4 CameraView = glm::mat4(1.0f);
	glm::mat4 CameraProjection = glm::mat4(1.0f);
};