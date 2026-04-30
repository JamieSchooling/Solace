#include "Rendering/FBO.h"

#include <glad/gl.h>

#include <iostream>

FBO::FBO(glm::ivec2 size)
{
	glCreateFramebuffers(1, &m_ID);

	m_Size = size;

	m_Target = std::make_shared<Texture>(m_Size);
	glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0, m_Target->GetID(), 0);

	GLenum status = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);
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
	if (m_ID > 0) glDeleteFramebuffers(1, &m_ID);
}

void FBO::Use() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FBO::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::Resize(glm::ivec2 size)
{
	m_Size = size;
	m_Target->Recreate(m_Size);
	glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0, m_Target->GetID(), 0);
}

std::shared_ptr<Texture> FBO::GetTarget()
{
	return m_Target;
}
