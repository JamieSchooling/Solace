#pragma once

#include <cstdint>

#include <glad/gl.h>
#include <vector>

enum VertexAttribType
{
	Short = GL_SHORT,
	UnsignedShort = GL_UNSIGNED_SHORT,
	Float = GL_FLOAT,
	Int = GL_INT,
	UnsignedInt = GL_UNSIGNED_INT,
	Double = GL_DOUBLE
};

struct VertexAttribute
{
	uint32_t size;
	VertexAttribType type;
	bool normalised;
	uint32_t offset;
};

class VAO
{
public:
	VAO(const std::vector<uint32_t>& indices);
	
	void AddVertexBuffer(const std::vector<float>& vertices, const std::vector<VertexAttribute>& attributes);
	void Use();
private:
	uint32_t m_ID = 0;
	uint32_t m_EBO = 0;
	std::vector<uint32_t> m_VBOs;

	uint32_t CalculateStride(const std::vector<VertexAttribute>& attributes);
};