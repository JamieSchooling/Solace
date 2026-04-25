#pragma once

#include "Core/Subsystem.h"
#include "Rendering/VAO.h"
#include "Rendering/Material.h"

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>

struct RenderItem
{
	VAO geometry;
	Material material;
	glm::mat4 transform = glm::mat4(1.0f);
};

using RenderQueue = std::vector<RenderItem>;

struct RenderSystemProps : public SubsystemParams
{
	std::shared_ptr<RenderQueue> renderQueue = nullptr;
};

class RenderSystem : public SingletonSubsystem<RenderSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void PreAppUpdate() override;
	void OnAppUpdate() override;

private:
	std::shared_ptr<RenderQueue> m_RenderQueue;
};