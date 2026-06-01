#include "Rendering/Texture.h"

#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

Texture::Texture(glm::ivec2 size, TextureFormat format)
{
	Init(size, format);
}

Texture::Texture(glm::ivec2 size, unsigned char* data)
{
	Init(size, TextureFormat::RGBA8, data);
}

Texture::Texture(std::filesystem::path imagePath)
{
	int width, height, numChannels;
	unsigned char* data = stbi_load(imagePath.string().c_str(), &width, &height, &numChannels, 4);
	Init({ width, height }, TextureFormat::RGBA8, data);
	stbi_image_free(data);
}

Texture::~Texture()
{
	Delete();
}

Texture& Texture::WhiteTexture()
{
	static unsigned char pixel[] =
	{
		255, 255, 255, 255
	};

	static Texture texture({ 1, 1 }, pixel);

	return texture;
}

void Texture::Recreate(glm::ivec2 size)
{
	Delete();
	Init(size);
}

void Texture::Recreate(std::filesystem::path imagePath)
{
	Delete();
	int width, height, numChannels;
	unsigned char* data = stbi_load(imagePath.string().c_str(), &width, &height, &numChannels, 4);
	Init({ width, height }, TextureFormat::RGBA8, data);
	stbi_image_free(data);
}

bool Texture::IsImageFile(const std::filesystem::path& path)
{
	return stbi_info(path.string().c_str(), nullptr, nullptr, nullptr) != 0;
}


void Texture::Init(glm::ivec2 size, TextureFormat format, unsigned char* data)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

	if (format == TextureFormat::Depth32F)
	{
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 

		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		/*float border[] = { 1.f, 1.f, 1.f, 1.f };
		glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, border);*/
	}
	else
	{
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	GLenum internalFormat = 0;
	switch (format)
	{
	case TextureFormat::RGBA8:
		internalFormat = GL_RGBA8;
		break;
	case TextureFormat::Depth32F:
		internalFormat = GL_DEPTH_COMPONENT32F;
		break;
	}

	glTextureStorage2D(m_id, 1, internalFormat, size.x, size.y);

	if (data && format == TextureFormat::RGBA8)
	{
		glTextureSubImage2D(m_id, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateTextureMipmap(m_id);
	}
}

void Texture::Delete()
{
	if (m_id)
	{
		glDeleteTextures(1, &m_id);
		m_id = 0;
	}
}