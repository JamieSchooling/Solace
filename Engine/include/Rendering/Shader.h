#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	void Use();
	inline unsigned int GetID() const { return m_ID; }

	void SetBool(const char* name, bool value) const;
	void SetInt(const char* name, int value) const;
	void SetFloat(const char* name, float value) const;
	void SetVec2(const char* name, const glm::vec2& value) const;
	void SetVec2(const char* name, float x, float y) const;
	void SetVec3(const char* name, const glm::vec3& value) const;
	void SetVec3(const char* name, float x, float y, float z) const;
	void SetVec4(const char* name, const glm::vec4& value) const;
	void SetVec4(const char* name, float x, float y, float z, float w);
	void SetMat2(const char* name, const glm::mat2& mat) const;
	void SetMat3(const char* name, const glm::mat3& mat) const;
	void SetMat4(const char* name, const glm::mat4& mat) const;

private:
	void CheckCompileErrors(uint32_t shader, const char* type);
	uint32_t m_ID;
	std::unordered_map<const char*, uint32_t> m_UniformLookup;
};