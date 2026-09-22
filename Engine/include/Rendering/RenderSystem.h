#pragma once

#include "Core/Subsystem.h"
#include "Rendering/FrameRenderData.h"
#include "Rendering/FBO.h"
#include "Rendering/UBO.h"

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <Rendering/Camera.h>
#include <Transform/Transform.h>

class RenderSystem : public SingletonSubsystem<RenderSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void PostAppUpdate() override;

	inline std::shared_ptr<FBO> GetDefaultFramebuffer() { return m_defaultFramebuffer; }

	void Render(Camera camera, Transform cameraTransform);

private:
	std::shared_ptr<FBO> m_defaultFramebuffer;
	std::unique_ptr<UBO> m_cameraUBO;
	std::unique_ptr<UBO> m_lightUBO;

	std::vector<RenderItem>* m_renderQueue;
	LightData* m_lightData;
	RenderView* m_shadowView;
	std::vector<std::pair<Camera, Transform>>* m_cameras;
};