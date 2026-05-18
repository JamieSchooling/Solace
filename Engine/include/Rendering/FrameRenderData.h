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
	glm::vec3 CameraPosition = glm::vec3(0.0f);
	glm::mat4 CameraView = glm::mat4(1.0f);
	glm::mat4 CameraProjection = glm::mat4(1.0f);
};

struct alignas(16) CameraUBOData
{
	glm::mat4 View = glm::mat4(1.0f);
	glm::mat4 Projection = glm::mat4(1.0f);
	glm::vec4 Position = glm::vec4(0.0f);
};

struct alignas(16) DirectionalLightUBOData
{
	glm::vec4 Colour;
	glm::vec4 Direction;
};

struct alignas(16) LightsUBOData
{
	DirectionalLightUBOData DLight;
};