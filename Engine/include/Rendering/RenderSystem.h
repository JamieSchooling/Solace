#pragma once

#include "Core/Subsystem.h"
#include "Rendering/FrameRenderData.h"
#include "Rendering/FBO.h"

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>

struct RenderSystemProps : public SubsystemParams
{
	FrameRenderData* renderData;
	std::shared_ptr<FBO> renderTarget;
};

class RenderSystem : public SingletonSubsystem<RenderSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void OnAppUpdate() override;

	inline std::shared_ptr<FBO> GetRenderTarget() { return m_renderTarget; }

private:
	FrameRenderData* m_renderData;
	std::shared_ptr<FBO> m_renderTarget;
};