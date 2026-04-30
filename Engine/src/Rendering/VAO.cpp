#include "Rendering/VAO.h"

#include <glad/gl.h>

VAO::VAO(const std::vector<uint32_t>& indices)
{
	glCreateVertexArrays(1, &m_ID);

	glCreateBuffers(1, &m_EBO);
	glNamedBufferStorage(m_EBO, sizeof(uint32_t) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(m_ID, m_EBO);

	m_Count = indices.size();
}

void VAO::AddVertexBuffer(const std::vector<float>& vertices, const std::vector<VertexAttribute>& attributes)
{
	uint32_t vbo = m_VBOs.emplace_back();
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	glVertexArrayVertexBuffer(m_ID, 0, vbo, 0, CalculateStride(attributes));

	for (size_t i = 0; i < attributes.size(); ++i)
	{
		glEnableVertexArrayAttrib(m_ID, i);
		glVertexArrayAttribFormat(m_ID, i, attributes[i].size, attributes[i].type, attributes[i].normalised ? GL_TRUE : GL_FALSE, attributes[i].offset);
		glVertexArrayAttribBinding(m_ID, i, 0);
	}
}

void VAO::Use() const
{
	glBindVertexArray(m_ID);
}

uint32_t VAO::CalculateStride(const std::vector<VertexAttribute>& attributes)
{
	uint32_t stride = 0;
	for (auto& attr : attributes)
	{
		uint32_t typeSize = 0;
		switch (attr.type)
		{
		case Short:
			typeSize = sizeof(short);
			break;
		case UnsignedShort:
			typeSize = sizeof(unsigned short);
			break;
		case Float:
			typeSize = sizeof(float);
			break;
		case Int:
			typeSize = sizeof(int32_t);
			break;
		case UnsignedInt:
			typeSize = sizeof(uint32_t);
			break;
		case Double:
			typeSize = sizeof(double);
			break;
		default:
			break;
		}

		stride += attr.size * typeSize;
	}

	return stride;
}
