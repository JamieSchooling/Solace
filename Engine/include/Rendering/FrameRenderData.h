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


struct alignas(16) CameraData
{
	glm::mat4 View = glm::mat4(1.0f);
	glm::mat4 Projection = glm::mat4(1.0f);
	glm::vec4 Position = glm::vec4(0.0f);
};

struct alignas(16) DirectionalLightData
{
	glm::vec4 Colour;
	glm::vec4 Direction;
};

struct alignas(16) LightData
{
	DirectionalLightData DLight;
};

struct FrameRenderData
{
	RenderQueue RenderQueue;
	CameraData Camera;
	LightData Lights;
};