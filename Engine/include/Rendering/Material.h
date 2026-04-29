#pragma once

#include "Rendering/Shader.h"

#include <iostream>

class Material
{
public:
	Material(std::shared_ptr<Shader> shader);

	template<typename T>
	void SetValue(const std::string& name, T data)
	{
		if (!m_UniformData.contains(name))
		{
			std::cout << "Material value [" << name << "] not set, uniform not found in shader." << std::endl;
			return;
		}

		m_UniformData[name] = data;
	}

	void Use();

private:
	std::shared_ptr<Shader> m_Shader;
	std::unordered_map<std::string, UniformData> m_UniformData;
};