#include "Rendering/TextureUnitAllocator.h"

void TextureUnitAllocator::Reset()
{
	m_next = 0;
}

uint32_t TextureUnitAllocator::GetNext(uint32_t textureID)
{
	if (m_boundTextures.contains(textureID)) { return m_boundTextures.at(textureID); }

	int slot = m_next++;

	m_boundTextures[textureID] = slot;

	return slot;
}
