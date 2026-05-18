#pragma once

#include <cstdint>

#include <glad/gl.h>

class UBO
{
public:
	UBO(uint32_t size, uint32_t binding);
	~UBO();

	template<typename T>
	void Upload(const T& data)
	{
		glNamedBufferSubData(
			m_id,
			0,
			sizeof(T),
			&data
		);
	}

private:
	uint32_t m_id = 0;
};