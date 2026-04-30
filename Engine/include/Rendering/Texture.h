#pragma once

#include <glm/glm.hpp>
#include <cstdint>

class Texture
{
public:
	Texture(glm::ivec2 size);
	~Texture();

	inline uint32_t GetID() const { return m_ID; }

	void Recreate(glm::ivec2 size);
private:
	uint32_t m_ID;

	void Init(glm::ivec2 size);
	void Delete();
};