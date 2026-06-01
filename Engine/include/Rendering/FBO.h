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

class FBO
{
public:
	FBO() { m_id = 0; }; // Default framebuffer
	FBO(glm::ivec2 size, AttachmentType attachmentType = AttachmentType::Colour);
	~FBO();
	void Use();
	void Unbind() const;
	void Resize(glm::ivec2 size);
	std::shared_ptr<Texture> GetTarget();
	glm::ivec2 GetSize() { return m_size; }
private:
	std::shared_ptr<Texture> m_target = nullptr;
	glm::ivec2 m_size = glm::ivec2(0, 0);
	uint32_t m_id = 0;
};