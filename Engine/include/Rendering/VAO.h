#pragma once

#include <cstdint>
#include <vector>

#include "Rendering/ShaderDataTypes.h"

struct VertexAttribute
{
	uint32_t size;
	ShaderDataType type;
	bool normalised;
	uint32_t offset;
};

class VAO
{
public:
	VAO(const std::vector<uint32_t>& indices);
	
	void AddVertexBuffer(const std::vector<float>& vertices, const std::vector<VertexAttribute>& attributes);
	inline uint32_t Count() const { return m_Count; }
	void Use() const;
private:
	uint32_t m_ID = 0;
	uint32_t m_EBO = 0;
	uint32_t m_Count = 0;
	std::vector<uint32_t> m_VBOs;

	uint32_t CalculateStride(const std::vector<VertexAttribute>& attributes);
};