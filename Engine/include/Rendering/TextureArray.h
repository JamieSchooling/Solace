#pragma once

#include <glm/glm.hpp>
#include <cstdint>

#include <filesystem>

#include "Rendering/TextureFormat.h"

class TextureArray
{
public:
	TextureArray(glm::ivec2 size, size_t arrayLength, TextureFormat format = TextureFormat::RGBA8);
	TextureArray(glm::ivec2 size, size_t arrayLength, unsigned char* data);
	TextureArray(size_t arrayLength, std::filesystem::path imagePath);
	~TextureArray();

	inline uint32_t GetID() const { return m_id; }

	void Recreate(glm::ivec2 size);
	void Recreate(std::filesystem::path imagePath);

private:
	uint32_t m_id;
	size_t m_arrayLength = 0;
	TextureFormat m_format;

	void Init(glm::ivec2 size, size_t arrayLength, TextureFormat format = TextureFormat::RGBA8, unsigned char* data = nullptr);
	void Delete();
};