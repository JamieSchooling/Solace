#pragma once

#include "Rendering/Shader.h"

#include <iostream>

class Material
{
public:
	Material(std::shared_ptr<Shader> shader);

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

private:
	std::shared_ptr<Shader> m_shader;
	std::unordered_map<std::string, UniformData> m_uniformData;
};