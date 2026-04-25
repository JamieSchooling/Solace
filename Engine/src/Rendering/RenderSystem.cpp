#include "Rendering/RenderSystem.h"
#include <iostream>

#include <glad/gl.h>

void RenderSystem::Start(const SubsystemParams& params)
{
	const RenderSystemProps& props = static_cast<const RenderSystemProps&>(params);

	m_RenderQueue = props.renderQueue;
}

void RenderSystem::Shutdown()
{
	m_RenderQueue = nullptr;
}

void RenderSystem::PreAppUpdate()
{
	if (m_RenderQueue->empty()) return;
}

void RenderSystem::OnAppUpdate()
{
	
	for (RenderItem& item : *m_RenderQueue)
	{
		// Render
		item.geometry.Use();
		item.material.Use();
		glDrawElements(GL_TRIANGLES, item.geometry.Count(), GL_UNSIGNED_INT, NULL);
	}
}
