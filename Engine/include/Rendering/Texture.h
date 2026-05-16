#pragma once

#include <glm/glm.hpp>
#include <cstdint>

#include <filesystem>

class Texture
{
public:
	Texture(glm::ivec2 size);
	Texture(std::filesystem::path imagePath);
	~Texture();

	inline uint32_t GetID() const { return m_id; }

	void Recreate(glm::ivec2 size);
	void Recreate(std::filesystem::path imagePath);
private:
	uint32_t m_id;

	void Init(glm::ivec2 size, unsigned char* data = nullptr);
	void Delete();
};