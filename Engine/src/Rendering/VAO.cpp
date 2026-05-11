#include "Rendering/VAO.h"

#include <glad/gl.h>

VAO::VAO(const std::vector<uint32_t>& indices)
{
	glCreateVertexArrays(1, &m_id);

	glCreateBuffers(1, &m_ebo);
	glNamedBufferStorage(m_ebo, sizeof(uint32_t) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(m_id, m_ebo);

	m_count = indices.size();
}

void VAO::AddVertexBuffer(const std::vector<float>& vertices, const std::vector<VertexAttribute>& attributes)
{
	uint32_t vbo = m_vbos.emplace_back();
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	glVertexArrayVertexBuffer(m_id, 0, vbo, 0, CalculateStride(attributes));

	for (size_t i = 0; i < attributes.size(); ++i)
	{
		glEnableVertexArrayAttrib(m_id, i);
		glVertexArrayAttribFormat(m_id, i, attributes[i].Size, attributes[i].Type, attributes[i].Normalised ? GL_TRUE : GL_FALSE, attributes[i].Offset);
		glVertexArrayAttribBinding(m_id, i, 0);
	}
}

void VAO::Use() const
{
	glBindVertexArray(m_id);
}

uint32_t VAO::CalculateStride(const std::vector<VertexAttribute>& attributes)
{
	uint32_t stride = 0;
	for (auto& attr : attributes)
	{
		uint32_t typeSize = 0;
		switch (attr.Type)
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

		stride += attr.Size * typeSize;
	}

	return stride;
}
