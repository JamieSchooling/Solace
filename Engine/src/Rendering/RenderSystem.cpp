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

	m_cameraUBO = std::make_unique<UBO>(sizeof(CameraUBOData), 0);
	m_lightUBO = std::make_unique<UBO>(sizeof(LightsUBOData), 1);
}

void RenderSystem::Shutdown()
{

}

void RenderSystem::OnAppUpdate()
{	
	m_renderTarget->Use();

	CameraUBOData camData{ m_renderData->CameraProjection, m_renderData->CameraView, glm::vec4(m_renderData->CameraPosition, 0.0f) };
	m_cameraUBO->Upload(camData);

	LightsUBOData lightData;
	lightData.DLight.Colour = glm::vec4(1.0f);
	lightData.DLight.Direction = glm::vec4(glm::normalize(glm::vec3(1.f, -2.5f, -2.f)), 0.0f);
	m_lightUBO->Upload(lightData);

	for (RenderItem& item : m_renderData->RenderQueue)
	{
		// Render
		//if (!item.Geometry) { continue; }
		item.Geometry->Use();

		//if (!item.Material) { continue; }
		item.Material->SetValue("u_model", item.Transform);
		item.Material->Use();
		glDrawElements(GL_TRIANGLES, item.Geometry->Count(), GL_UNSIGNED_INT, NULL);
	}

	m_renderTarget->Unbind();
}
