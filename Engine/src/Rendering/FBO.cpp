#include "Rendering/FBO.h"

#include <glad/gl.h>

#include <iostream>

FBO::FBO(glm::ivec2 size, std::vector<FBOAttachment> attachments)
{
	glCreateFramebuffers(1, &m_id);

	m_size = size;
	uint32_t colourIndex = 0;
	for (auto attachment : attachments)
	{
		switch (attachment.Type)
		{
		case AttachmentType::Colour:
			m_colourAttachments.push_back(std::make_shared<Texture>(m_size));
			glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0 + colourIndex, m_colourAttachments.back()->GetID(), 0);
			colourIndex++;
			break;
		case AttachmentType::Depth:
			m_depthAttachment = std::make_shared<Texture>(m_size, TextureFormat::Depth32F);
			glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT, m_depthAttachment->GetID(), 0);
			glNamedFramebufferDrawBuffer(m_id, GL_NONE);
			glNamedFramebufferReadBuffer(m_id, GL_NONE);
			break;
		}
	}

	std::vector<GLenum> drawBuffers;

	for (uint32_t i = 0; i < m_colourAttachments.size(); i++)
	{
		drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}

	if (!drawBuffers.empty())
	{
		glNamedFramebufferDrawBuffers(
			m_id,
			static_cast<GLsizei>(drawBuffers.size()),
			drawBuffers.data());
	}
	else
	{
		glNamedFramebufferDrawBuffer(m_id, GL_NONE);
		glNamedFramebufferReadBuffer(m_id, GL_NONE);
	}

	GLenum status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error: ";

		switch (status) {
			case GL_FRAMEBUFFER_UNDEFINED: std::cout << "GL_FRAMEBUFFER_UNDEFINED" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl; break;
			case GL_FRAMEBUFFER_UNSUPPORTED: std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl; break;
			default: std::cout << "Unknown Framebuffer error." << std::endl; break;
		}
	}
}

FBO::~FBO()
{
	if (m_id > 0) glDeleteFramebuffers(1, &m_id);
}

void FBO::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	if (m_id == 0)
	{
		m_size = glm::ivec2(Window::Get().GetWidth(), Window::Get().GetHeight());
	}
	glViewport(0, 0, m_size.x, m_size.y);
}

void FBO::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::Resize(glm::ivec2 size)
{
	m_size = size;
	uint32_t colourIndex = 0;
	for (auto attachment : m_colourAttachments)
	{
		attachment->Recreate(m_size);
		glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0 + colourIndex, attachment->GetID(), 0);
		colourIndex++;
	}

	m_depthAttachment = std::make_shared<Texture>(m_size, TextureFormat::Depth32F);
	glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT, m_depthAttachment->GetID(), 0);
	glNamedFramebufferDrawBuffer(m_id, GL_NONE);
	glNamedFramebufferReadBuffer(m_id, GL_NONE);

	std::vector<GLenum> drawBuffers;
	for (uint32_t i = 0; i < m_colourAttachments.size(); i++)
	{
		drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	if (!drawBuffers.empty())
	{
		glNamedFramebufferDrawBuffers(
			m_id,
			static_cast<GLsizei>(drawBuffers.size()),
			drawBuffers.data());
	}
	else
	{
		glNamedFramebufferDrawBuffer(m_id, GL_NONE);
		glNamedFramebufferReadBuffer(m_id, GL_NONE);
	}
}

std::shared_ptr<Texture> FBO::GetTarget(uint32_t index)
{
	return m_colourAttachments.at(index);
}

std::shared_ptr<Texture> FBO::GetDepthTarget()
{
	return m_depthAttachment;
}
