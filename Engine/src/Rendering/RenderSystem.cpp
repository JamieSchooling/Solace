#include "Rendering/RenderSystem.h"
#include <iostream>

void RenderSystem::Start(const SubsystemParams& params)
{
	const RenderSystemProps& props = static_cast<const RenderSystemProps&>(params);

	m_RenderQueue = props.renderQueue;
}

void RenderSystem::Shutdown()
{
	m_RenderQueue = nullptr;
}

void RenderSystem::OnAppUpdate()
{
	auto& queue = *m_RenderQueue;
	if (queue.empty()) return;
	for (const RenderItem& item : queue)
	{
		// Render
	}
}
