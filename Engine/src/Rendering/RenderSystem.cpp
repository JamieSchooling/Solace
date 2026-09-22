#include "Rendering/RenderSystem.h"
#include <iostream>

#include <glad/gl.h>
#include <Rendering/TextureUnitAllocator.h>
#include <Scenes/SceneSystem.h>

#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

void RenderSystem::Start(const SubsystemParams& params)
{
	m_cameraUBO = std::make_unique<UBO>(sizeof(CameraData), 0);
	m_lightUBO = std::make_unique<UBO>(sizeof(LightData), 1);

	m_defaultFramebuffer = std::make_shared<FBO>(glm::ivec2(Window::Get().GetWidth(), Window::Get().GetWidth()), std::vector<FBOAttachment>{ {AttachmentType::Colour}, { AttachmentType::Depth } });
}

void RenderSystem::Shutdown()
{

}

void RenderSystem::PostAppUpdate()
{	
	TextureUnitAllocator::Reset();

	m_renderQueue = &SceneSystem::Get().FetchVisibleObjectData();
	m_lightData = &SceneSystem::Get().FetchLightData();
	m_shadowView = &SceneSystem::Get().GetShadowView();
	m_cameras = &SceneSystem::Get().FetchActiveCameras();

	m_lightUBO->Upload(*m_lightData);

	for (auto& [camera, transform] : *m_cameras)
	{
		Render(camera, transform);
	}
}

void RenderSystem::Render(Camera camera, Transform cameraTransform)
{
	std::shared_ptr<FBO> renderTarget = nullptr;

	if (!camera.RenderTarget)
	{
		renderTarget = GetDefaultFramebuffer();
	}
	else
	{
		renderTarget = camera.RenderTarget;
	}

	if (!renderTarget) return;

	CameraData cameraData;
	cameraData.Projection = camera.GetProjection();
	cameraData.View = camera.GetView(cameraTransform);
	cameraData.Position = glm::vec4(cameraTransform.Position, 0.0f);

	{
		ZoneScopedN("ShadowPass");
		//TracyGpuZone("ShadowPass");
		m_shadowView->RenderTarget->Use();
		m_cameraUBO->Upload(m_shadowView->Camera);
		//glDepthFunc(GL_GREATER);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);

		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (RenderItem& depthItem : *m_renderQueue)
		{
			TracyGpuZone("ShadowPass::DrawEntity");
			// Render
			if (!depthItem.DepthGeometry) continue;
			depthItem.DepthGeometry->Use();
			if (depthItem.DepthMaterial)
			{
				depthItem.DepthMaterial->SetValue("u_model", depthItem.Transform);
				depthItem.DepthMaterial->Use();
			}
			glDrawElements(GL_TRIANGLES, depthItem.Geometry->Count(), GL_UNSIGNED_INT, NULL);
		}
		m_shadowView->RenderTarget->Unbind();
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	TracyGpuZone("RenderPass");
	renderTarget->Use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_cameraUBO->Upload(cameraData);

	for (RenderItem& item : *m_renderQueue)
	{
		TracyGpuZone("RenderPass::DrawEntity");
		// Render
		if (!item.Geometry) continue;
		item.Geometry->Use();
		if (item.Material)
		{
			item.Material->SetValue("u_model", item.Transform);
			item.Material->SetValue("u_shadowMap", m_shadowView->RenderTarget->GetDepthTarget());
			item.Material->SetValue("u_lightSpaceTransform", m_shadowView->Camera.Projection * m_shadowView->Camera.View);
			item.Material->Use();
		}
		glDrawElements(GL_TRIANGLES, item.Geometry->Count(), GL_UNSIGNED_INT, NULL);
	}

	renderTarget->Unbind();
}