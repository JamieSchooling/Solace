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
}

void RenderSystem::Shutdown()
{

}

void RenderSystem::OnAppUpdate()
{	
	m_renderTarget->Use();

	for (RenderItem& item : m_renderData->RenderQueue)
	{
		// Render
		//if (!item.Geometry) { continue; }
		item.Geometry->Use();

		//if (!item.Material) { continue; }
		item.Material->SetValue("u_Model", item.Transform);
		item.Material->SetValue("u_MVP", m_renderData->CameraProjection * m_renderData->CameraView * item.Transform);
		item.Material->Use();
		glDrawElements(GL_TRIANGLES, item.Geometry->Count(), GL_UNSIGNED_INT, NULL);
	}

	m_renderTarget->Unbind();
}
