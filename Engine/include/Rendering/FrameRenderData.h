#pragma once

#include "Rendering/VAO.h"
#include "Rendering/FBO.h"
#include "Rendering/Material.h"

struct RenderItem
{
	std::shared_ptr<VAO> Geometry;
	std::shared_ptr<Material> Material;
	glm::mat4 Transform = glm::mat4(1.0f);
};

struct alignas(16) CameraData
{
	glm::mat4 Projection = glm::mat4(1.0f);
	glm::mat4 View = glm::mat4(1.0f);
	glm::vec4 Position = glm::vec4(0.0f);
};

struct alignas(16) DirectionalLightData
{
	glm::vec4 Colour;
	glm::vec4 Direction;
	float Intensity;
};

struct alignas(16) PointLightData
{
	glm::vec4 Colour;
	glm::vec4 Position;
	float Radius;
	float Intensity;
};

constexpr int MAX_POINT_LIGHTS = 32;
struct alignas(16) LightData
{
	DirectionalLightData DLight;
	PointLightData PLights[MAX_POINT_LIGHTS];
	int NumPointLights;
};

struct RenderView
{
	CameraData Camera;
	std::shared_ptr<FBO> RenderTarget;
};

struct FrameRenderData
{
	RenderView ShadowView;
	std::vector<RenderItem> ShadowQueue;
	std::vector<RenderItem> RenderQueue;
	std::vector<RenderView> RenderViews;
	LightData Lights;
};