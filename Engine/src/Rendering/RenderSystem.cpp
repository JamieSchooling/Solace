#include "Rendering/RenderSystem.h"
#include <iostream>

#include <glad/gl.h>

void RenderSystem::Start(const SubsystemParams& params)
{
	const RenderSystemProps& props = static_cast<const RenderSystemProps&>(params);

	m_renderData = props.renderData;
	m_renderTarget = props.renderTarget;
	if (!m_renderTarget)
	{
		m_renderTarget = std::make_shared<FBO>();
	}

	m_cameraUBO = std::make_unique<UBO>(sizeof(CameraData), 0);
	m_lightUBO = std::make_unique<UBO>(sizeof(LightData), 1);
}

void RenderSystem::Shutdown()
{

}

void RenderSystem::OnAppUpdate()
{	
	m_renderTarget->Use();

	m_cameraUBO->Upload(m_renderData->Camera);
	m_lightUBO->Upload(m_renderData->Lights);

	for (RenderItem& item : m_renderData->RenderQueue)
	{
		// Render
		if (!item.Geometry) continue;
		item.Geometry->Use();
		item.Material->SetValue("u_model", item.Transform);
		item.Material->Use();
		glDrawElements(GL_TRIANGLES, item.Geometry->Count(), GL_UNSIGNED_INT, NULL);
	}

	m_renderTarget->Unbind();
}
