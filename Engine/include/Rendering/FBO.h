#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <variant>

#include "Rendering/Texture.h"
#include "Rendering/Window.h"
#include "Rendering/TextureArray.h"

enum class AttachmentType
{
	Colour,
	Depth,
	ColourArray,
	DepthArray,
};
struct FBOAttachment
{
	AttachmentType Type;
	size_t ArrayLength;
};

using TexturePtr = std::variant<std::shared_ptr<Texture>, std::shared_ptr<TextureArray>>;

class FBO
{
public:
	FBO() { m_id = 0; }; // Default framebuffer
	FBO(glm::ivec2 size, std::vector<FBOAttachment> attachments = { {AttachmentType::Colour} });
	~FBO();
	void Use();
	void BindLayer(uint32_t attachmentIndex, uint32_t layer);
	void BindDepthLayer(uint32_t layer);
	void Unbind() const;
	void Resize(glm::ivec2 size);

	TexturePtr GetTarget(uint32_t index = 0);
	TexturePtr GetDepthTarget();
	glm::ivec2 GetSize() { return m_size; }
	inline uint32_t GetID() const { return m_id; }
private:
	std::vector<TexturePtr> m_colourAttachments;
	TexturePtr m_depthAttachment;
	glm::ivec2 m_size = glm::ivec2(0, 0);
	uint32_t m_id = 0;
};