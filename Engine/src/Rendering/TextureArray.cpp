#include "Rendering/TextureArray.h"

#include <glad/gl.h>

#include <stb/stb_image.h>

TextureArray::TextureArray(glm::ivec2 size, size_t arrayLength, TextureFormat format)
{
	Init(size, arrayLength, format);
}

TextureArray::TextureArray(glm::ivec2 size, size_t arrayLength, unsigned char* data)
{
	Init(size, arrayLength, TextureFormat::RGBA8, data);
}

TextureArray::TextureArray(size_t arrayLength, std::filesystem::path imagePath)
{
	int width, height, numChannels;
	unsigned char* data = stbi_load(imagePath.string().c_str(), &width, &height, &numChannels, 4);
	Init({ width, height }, arrayLength, TextureFormat::RGBA8, data);
	stbi_image_free(data);
}

TextureArray::~TextureArray()
{
	Delete();
}

void TextureArray::Recreate(glm::ivec2 size)
{
	Delete();
	Init(size, m_arrayLength, m_format);
}

void TextureArray::Recreate(std::filesystem::path imagePath)
{
	Delete();
	int width, height, numChannels;
	unsigned char* data = stbi_load(imagePath.string().c_str(), &width, &height, &numChannels, 4);
	Init({ width, height }, m_arrayLength, TextureFormat::RGBA8, data);
	stbi_image_free(data);
}

void TextureArray::Init(glm::ivec2 size, size_t arrayLength, TextureFormat format, unsigned char* data)
{
	m_arrayLength = arrayLength;
	m_format = format;

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_id);

	if (format == TextureFormat::Depth32F)
	{
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

	glTextureStorage3D(m_id, 1, internalFormat, size.x, size.y, arrayLength);

	if (data && format == TextureFormat::RGBA8)
	{
		for (size_t i = 0; i < arrayLength; ++i)
		{
			glTextureSubImage3D(m_id, 0, 0, 0, 0, size.x, size.y, i, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateTextureMipmap(m_id);
		}
	}
}

void TextureArray::Delete()
{
	if (m_id)
	{
		glDeleteTextures(1, &m_id);
		m_id = 0;
	}
}
