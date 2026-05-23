#pragma once

#include "Rendering/Shader.h"

#include <iostream>

enum MaterialInitFlags
{
	None = 0,
	InitWithDefaultValues = 1 << 0,
};

class Material
{
public:
	Material(std::shared_ptr<Shader> shader, MaterialInitFlags flags = MaterialInitFlags::None);

	template<typename T>
	void SetValue(const std::string& m_name, T data)
	{
		if (!m_uniformData.contains(m_name))
		{
			std::cout << "Material value [" << m_name << "] not set, uniform not found in shader." << std::endl;
			return;
		}

		m_uniformData[m_name] = data;
	}

	void Use();


	std::pair<const char*, const char*> GetShaderPaths() const { return m_shader->GetShaderPaths(); }
	std::unordered_map<std::string, UniformData> GetUniformData() const { return m_uniformData; }
	UniformDescription GetUniformDescription(const std::string& name) { return m_shader->m_uniformLookup.at(name); }

private:
	std::shared_ptr<Shader> m_shader;
	std::unordered_map<std::string, UniformData> m_uniformData;

	void SetDefaultValue(const std::string& name, UniformDescription desc);
};