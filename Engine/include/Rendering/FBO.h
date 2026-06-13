#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Rendering/Texture.h"
#include "Rendering/Window.h"

enum class AttachmentType
{
	Colour,
	Depth
};
struct FBOAttachment
{
	AttachmentType Type;
};

class FBO
{
public:
	FBO() { m_id = 0; }; // Default framebuffer
	FBO(glm::ivec2 size, std::vector<FBOAttachment> attachments = { {AttachmentType::Colour} });
	~FBO();
	void Use();
	void Unbind() const;
	void Resize(glm::ivec2 size);
	std::shared_ptr<Texture> GetTarget(uint32_t index = 0);
	std::shared_ptr<Texture> GetDepthTarget();
	glm::ivec2 GetSize() { return m_size; }
private:
	std::vector<std::shared_ptr<Texture>> m_colourAttachments;
	std::shared_ptr<Texture> m_depthAttachment;
	glm::ivec2 m_size = glm::ivec2(0, 0);
	uint32_t m_id = 0;
};