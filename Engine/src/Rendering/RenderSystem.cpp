#include "Rendering/RenderSystem.h"
#include <iostream>

#include <glad/gl.h>
#include <Rendering/TextureUnitAllocator.h>
#include <variant>
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
	float cameraAspect = cameraData.Projection[1][1] / cameraData.Projection[0][0];

	std::vector<float> cascadeSplits;
	cascadeSplits.reserve(camera.ShadowCascadeCount);

	std::vector<glm::mat4> lightSpaceTransforms;
	lightSpaceTransforms.reserve(camera.ShadowCascadeCount);

	{
		ZoneScopedN("ShadowPass");
		//TracyGpuZone("ShadowPass");
		m_shadowView->RenderTarget = std::make_shared<FBO>(glm::ivec2(2048, 2048), std::vector<FBOAttachment>{ {AttachmentType::DepthArray, (size_t)camera.ShadowCascadeCount} });
		m_shadowView->RenderTarget->Use();

		float previousFarDist = camera.Near;
		float range = camera.Far - camera.Near;
		float ratio = camera.Far / camera.Near;
		for (size_t i = 0; i < camera.ShadowCascadeCount; ++i)
		{
			float p = (i + 1) / (float)camera.ShadowCascadeCount;
			float log = camera.Near * std::pow(ratio, p);
			float uniform = camera.Near + range * p;
			float d = camera.ShadowCascadeDistribution * (log - uniform) + uniform;
			float cascadeSplit = (d - camera.Near) / range;

			float currentFarDist = camera.Near + (cascadeSplit * range);

			float cascadeNear = previousFarDist;
			float cascadeFar = currentFarDist;

			cascadeSplits.push_back(currentFarDist);
			previousFarDist = currentFarDist;

			glm::mat4 cascadeProj = glm::perspective(glm::radians(camera.FOV), cameraAspect, cascadeNear, cascadeFar);
			std::vector<glm::vec4> corners = GetFrustumCornersWorldSpace(cascadeProj, cameraData.View);

			glm::vec3 center = glm::vec3(0, 0, 0);
			for (const auto& v : corners)
			{
				center += glm::vec3(v);
			}
			center /= corners.size();

			const auto lightView = glm::lookAt(
				center + glm::vec3(-m_lightData->DLight.Direction),
				center,
				glm::vec3(0.0f, 1.0f, 0.0f)
			);

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::lowest();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::lowest();
			for (const auto& v : corners)
			{
				const auto trf = lightView * v;
				minX = std::min(minX, trf.x);
				maxX = std::max(maxX, trf.x);
				minY = std::min(minY, trf.y);
				maxY = std::max(maxY, trf.y);
			}

			float minZ = -200.0f;
			float maxZ = 200.0f;

			const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

			m_shadowView->Camera.Position = glm::vec4(center, 0.0f);
			m_shadowView->Camera.View = lightView;
			m_shadowView->Camera.Projection = lightProjection;
			lightSpaceTransforms.push_back(lightProjection * lightView);

			m_shadowView->RenderTarget->BindDepthLayer(i);

			m_cameraUBO->Upload(m_shadowView->Camera);
			//glDepthFunc(GL_GREATER);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glDepthMask(GL_TRUE);
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);

			float clearDepthValue = 1.0f;
			glClearNamedFramebufferfv(m_shadowView->RenderTarget->GetID(), GL_DEPTH, 0, &clearDepthValue);

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
			item.Material->SetValue("u_shadowMap", std::get<std::shared_ptr<TextureArray>>(m_shadowView->RenderTarget->GetDepthTarget()));
			for (size_t i = 0; i < lightSpaceTransforms.size(); ++i)
			{
				item.Material->SetValue("u_lightSpaceTransforms[" + std::to_string(i) + "]", lightSpaceTransforms[i]);
			}
			for (size_t i = 0; i < cascadeSplits.size(); ++i)
			{
				item.Material->SetValue("u_cascadeSplits[" + std::to_string(i) + "]", cascadeSplits[i]);
				item.Material->SetValue("u_cascadeSplits[" + std::to_string(i) + "]", cascadeSplits[i]);
			}
			item.Material->SetValue("u_numShadowCascades", (int)cascadeSplits.size());
			item.Material->Use();
		}
		glDrawElements(GL_TRIANGLES, item.Geometry->Count(), GL_UNSIGNED_INT, NULL);
	}

	renderTarget->Unbind();
}

std::vector<glm::vec4> RenderSystem::GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
	const auto inv = glm::inverse(proj * view);

	std::vector<glm::vec4> frustumCorners;
	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				const glm::vec4 pt =
					inv * glm::vec4(
						2.0f * x - 1.0f,
						2.0f * y - 1.0f,
						2.0f * z - 1.0f,
						1.0f);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}