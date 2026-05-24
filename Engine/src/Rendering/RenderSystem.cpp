#include "Rendering/RenderSystem.h"
#include <iostream>

#include <glad/gl.h>

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
	m_lightUBO->Upload(m_renderData->Lights);

	for (auto view : m_renderData->RenderViews)
	{
		view.RenderTarget->Use(); 
		m_cameraUBO->Upload(view.Camera);

		for (RenderItem& item : m_renderData->RenderQueue)
		{
			// Render
			if (!item.Geometry) continue;
			item.Geometry->Use();
			item.Material->SetValue("u_model", item.Transform);
			item.Material->Use();
			glDrawElements(GL_TRIANGLES, item.Geometry->Count(), GL_UNSIGNED_INT, NULL);
		}

		view.RenderTarget->Unbind();
	}	
}
