#pragma once
#include <cstdint>

#include <unordered_map>

class TextureUnitAllocator
{
public:
	static void Reset();
	static uint32_t GetNext(uint32_t textureID);

private:
	static inline std::unordered_map<uint32_t, uint32_t> m_boundTextures;
	static inline uint32_t m_next = 0;
};