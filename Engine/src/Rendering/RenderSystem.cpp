#include "Rendering/RenderSystem.h"
#include <iostream>

#include <glad/gl.h>

void RenderSystem::Start(const SubsystemParams& params)
{
	const RenderSystemProps& props = static_cast<const RenderSystemProps&>(params);

	m_RenderData = props.renderData;
	m_RenderTarget = props.renderTarget;
	if (!m_RenderTarget)
	{
		m_RenderTarget = std::make_shared<FBO>();
	}
}

void RenderSystem::Shutdown()
{

}

void RenderSystem::OnAppUpdate()
{	
	m_RenderTarget->Use();

	for (RenderItem& item : m_RenderData->renderQueue)
	{
		// Render
		item.geometry->Use();
		item.material->SetValue("u_Model", item.transform);
		item.material->SetValue("u_MVP", m_RenderData->cameraProjection * m_RenderData->cameraView * item.transform);
		item.material->Use();
		glDrawElements(GL_TRIANGLES, item.geometry->Count(), GL_UNSIGNED_INT, NULL);
	}

	m_RenderTarget->Unbind();
}
