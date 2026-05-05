#pragma once

#include <cstdint>

using TypeID = std::uint64_t;

// FNV-1a hash function
constexpr TypeID Hash(const char* str)
{
	TypeID hash = 14695981039346656037ull;
	while (*str)
	{
		hash ^= static_cast<unsigned char>(*str++);
		hash *= 1099511628211ull;
	}
	return hash;
}