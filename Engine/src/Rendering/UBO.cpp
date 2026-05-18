#include "Rendering/UBO.h"

#include <glad/gl.h>

UBO::UBO(uint32_t size, uint32_t binding)
{
	glCreateBuffers(1, &m_id);

	glNamedBufferStorage(
		m_id,
		size,
		nullptr,
		GL_DYNAMIC_STORAGE_BIT
	);

	glBindBufferBase(
		GL_UNIFORM_BUFFER,
		binding,
		m_id
	);
}

UBO::~UBO()
{
	glDeleteBuffers(1, &m_id);
}