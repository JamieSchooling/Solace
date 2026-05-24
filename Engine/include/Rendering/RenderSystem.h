#pragma once

#include "Core/Subsystem.h"
#include "Rendering/FrameRenderData.h"
#include "Rendering/FBO.h"
#include "Rendering/UBO.h"

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>

class RenderSystem : public SingletonSubsystem<RenderSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void PostAppUpdate() override;

	inline void SetFrameRenderData(FrameRenderData& frame) { m_renderData = &frame; }

private:
	FrameRenderData* m_renderData;
	std::unique_ptr<UBO> m_cameraUBO;
	std::unique_ptr<UBO> m_lightUBO;
};