#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Rendering/Texture.h"

class FBO
{
public:
	FBO() { m_ID = 0; }; // Default framebuffer
	FBO(glm::ivec2 size);
	~FBO();
	void Use() const;
	void Unbind() const;
	void Resize(glm::ivec2 size);
	std::shared_ptr<Texture> GetTarget();
	glm::ivec2 GetSize() { return m_Size; }
private:
	std::shared_ptr<Texture> m_Target = nullptr;
	glm::ivec2 m_Size = glm::ivec2(0, 0);
	uint32_t m_ID = 0;
};