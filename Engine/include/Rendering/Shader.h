#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

#include <glm/glm.hpp>

#include "Rendering/ShaderDataTypes.h"
#include "Rendering/Texture.h"
#include "Assets/AssetHandle.h"

using UniformData = std::variant<std::monostate, bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4, AssetHandle, std::shared_ptr<Texture>>;

struct UniformDescription
{
	uint32_t location;
	ShaderDataType type;
};

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	void Use();
	inline unsigned int GetID() const { return m_id; }

	void SetUniform(const std::string& m_name, UniformData uniformData);

	std::pair<std::string, std::string> GetShaderPaths() { return { m_vertexPath, m_fragmentPath }; };

private:
	void UploadUniform(uint32_t location, bool value) const;
	void UploadUniform(uint32_t location, int value) const;
	void UploadUniform(uint32_t location, float value) const;
	void UploadUniform(uint32_t location, const glm::vec2& value) const;
	void UploadUniform(uint32_t location, const glm::vec3& value) const;
	void UploadUniform(uint32_t location, const glm::vec4& value) const;
	void UploadUniform(uint32_t location, const glm::mat3& mat) const;
	void UploadUniform(uint32_t location, const glm::mat4& mat) const; 
	void UploadUniform(uint32_t location, const uint32_t textureID, const uint32_t slot) const;
	void CheckCompileErrors(uint32_t shader, const char* type);
private:
	uint32_t m_id;
	std::unordered_map<std::string, UniformDescription> m_uniformLookup;
	std::string m_vertexPath;
	std::string m_fragmentPath;

	friend class Material;
};