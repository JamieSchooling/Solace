#include "Rendering/RenderSystem.h"
#include <iostream>

#include <glad/gl.h>
#include <Rendering/TextureUnitAllocator.h>

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
	m_lightUBO->Upload(m_renderData->Lights);

	{
		m_renderData->ShadowView.RenderTarget->Use();
		m_cameraUBO->Upload(m_renderData->ShadowView.Camera);
		//glDepthFunc(GL_GREATER);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);

		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (RenderItem& depthItem : m_renderData->ShadowQueue)
		{
			// Render
			if (!depthItem.Geometry) continue;
			depthItem.Geometry->Use();
			if (depthItem.Material)
			{
				depthItem.Material->SetValue("u_model", depthItem.Transform);
				depthItem.Material->Use();
			}
			glDrawElements(GL_TRIANGLES, depthItem.Geometry->Count(), GL_UNSIGNED_INT, NULL);
		}
		m_renderData->ShadowView.RenderTarget->Unbind();
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
		view.RenderTarget->Use();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cameraUBO->Upload(view.Camera);

		for (RenderItem& item : m_renderData->RenderQueue)
		{
			// Render
			if (!item.Geometry) continue;
			item.Geometry->Use();
			if (item.Material)
			{
				item.Material->SetValue("u_model", item.Transform);
				item.Material->SetValue("u_shadowMap", m_renderData->ShadowView.RenderTarget->GetDepthTarget());
				item.Material->SetValue("u_lightSpaceTransform", m_renderData->ShadowView.Camera.Projection * m_renderData->ShadowView.Camera.View);
				item.Material->Use();
			}
			glDrawElements(GL_TRIANGLES, item.Geometry->Count(), GL_UNSIGNED_INT, NULL);
		}

		view.RenderTarget->Unbind();
	}	
}
