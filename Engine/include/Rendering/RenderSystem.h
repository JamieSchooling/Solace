#pragma once

#include "Core/Subsystem.h"
#include "Rendering/FrameRenderData.h"

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>

struct RenderSystemProps : public SubsystemParams
{
	FrameRenderData* renderData;
};

class RenderSystem : public SingletonSubsystem<RenderSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void OnAppUpdate() override;

private:
	FrameRenderData* m_RenderData;
};