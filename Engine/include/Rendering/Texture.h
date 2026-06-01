#pragma once

#include <glm/glm.hpp>
#include <cstdint>

#include <filesystem>

enum class TextureFormat
{
	RGBA8,
	Depth32F
};

class Texture
{
public:
	Texture(glm::ivec2 size, TextureFormat format = TextureFormat::RGBA8);
	Texture(glm::ivec2 size, unsigned char* data);
	Texture(std::filesystem::path imagePath);
	~Texture();

	static Texture& WhiteTexture();

	inline uint32_t GetID() const { return m_id; }

	void Recreate(glm::ivec2 size);
	void Recreate(std::filesystem::path imagePath);

	static bool IsImageFile(const std::filesystem::path& path);
private:
	uint32_t m_id;

	void Init(glm::ivec2 size, TextureFormat format = TextureFormat::RGBA8, unsigned char* data = nullptr);
	void Delete();
};