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
}

void RenderSystem::Shutdown()
{

}

void RenderSystem::PostAppUpdate()
{	
	TextureUnitAllocator::Reset();

	auto renderQueue = SceneSystem::Get().FetchRenderQueue();
	auto lights = SceneSystem::Get().FetchLightData();
	auto shadowView = SceneSystem::Get().GetShadowView();

	m_lightUBO->Upload(lights);

	{
		ZoneScopedN("ShadowPass");
		//TracyGpuZone("ShadowPass");
		shadowView.RenderTarget->Use();
		m_cameraUBO->Upload(shadowView.Camera);
		//glDepthFunc(GL_GREATER);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);

		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (RenderItem& depthItem : renderQueue)
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
		shadowView.RenderTarget->Unbind();
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (auto view : m_renderData->RenderViews)
	{
		TracyGpuZone("RenderPass");
		view.RenderTarget->Use();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cameraUBO->Upload(view.Camera);

		for (RenderItem& item : renderQueue)
		{
			TracyGpuZone("RenderPass::DrawEntity");
			// Render
			if (!item.Geometry) continue;
			item.Geometry->Use();
			if (item.Material)
			{
				item.Material->SetValue("u_model", item.Transform);
				item.Material->SetValue("u_shadowMap", shadowView.RenderTarget->GetDepthTarget());
				item.Material->SetValue("u_lightSpaceTransform", shadowView.Camera.Projection * shadowView.Camera.View);
				item.Material->Use();
			}
			glDrawElements(GL_TRIANGLES, item.Geometry->Count(), GL_UNSIGNED_INT, NULL);
		}

		view.RenderTarget->Unbind();
	}	
}
