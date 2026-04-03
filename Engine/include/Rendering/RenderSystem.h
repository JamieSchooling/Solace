#pragma once

#include "Core/Subsystem.h"

#include <memory>
#include <vector>

struct RenderItem
{
	// mat4 transform;
	// VAO geometry;
	// Material material // Contains shader
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

	void OnAppUpdate() override;

private:
	std::shared_ptr<RenderQueue> m_RenderQueue;
};