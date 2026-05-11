#include "Rendering/Texture.h"

#include <glad/gl.h>

Texture::Texture(glm::ivec2 size)
{
	Init(size);
}

Texture::~Texture()
{
	Delete();
}

void Texture::Recreate(glm::ivec2 size)
{
	Delete();
	Init(size);
}

void Texture::Init(glm::ivec2 size)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

	glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureStorage2D(m_id, 1, GL_RGBA8, size.x, size.y);
}

void Texture::Delete()
{
	glDeleteTextures(1, &m_id);
}