#pragma once

#include <cstdint>
#include <vector>

#include "Rendering/ShaderDataTypes.h"

struct VertexAttribute
{
	uint32_t Size;
	ShaderDataType Type;
	bool Normalised;
	uint32_t Offset;
};

class VAO
{
public:
	VAO(const std::vector<uint32_t>& indices);
	
	void AddVertexBuffer(const std::vector<float>& vertices, const std::vector<VertexAttribute>& attributes);
	inline uint32_t Count() const { return m_count; }
	void Use() const;
private:
	uint32_t m_id = 0;
	uint32_t m_ebo = 0;
	uint32_t m_count = 0;
	std::vector<uint32_t> m_vbos;

	uint32_t CalculateStride(const std::vector<VertexAttribute>& attributes);
};