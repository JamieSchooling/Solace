#include "Rendering/Texture.h"

#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

Texture::Texture(glm::ivec2 size)
{
	Init(size);
}

Texture::Texture(std::filesystem::path imagePath)
{
	int width, height, numChannels;
	unsigned char* data = stbi_load(imagePath.string().c_str(), &width, &height, &numChannels, 0);
	if (data)
	{
		std::cout << "Image: " << imagePath << " loaded." << std::endl;
		std::cout << "Width: " << width << "Height: " << height << std::endl;
	}
	Init({ width, height }, data); 
	stbi_image_free(data);
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

void Texture::Recreate(std::filesystem::path imagePath)
{
	Delete();
	int width, height, numChannels;
	unsigned char* data = stbi_load(imagePath.string().c_str(), &width, &height, &numChannels, 4);
	Init({ width, height }, data);
	stbi_image_free(data);
}


void Texture::Init(glm::ivec2 size, unsigned char* data)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

	glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureStorage2D(m_id, 1, GL_RGBA8, size.x, size.y);

	if (data)
	{
		glTextureSubImage2D(m_id, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateTextureMipmap(m_id);
	}
}

void Texture::Delete()
{
	glDeleteTextures(1, &m_id);
}