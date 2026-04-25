#include "Rendering/Material.h"

Material::Material(std::shared_ptr<Shader> shader) : m_Shader(shader)
{
	for (auto& pair : m_Shader->m_UniformLookup)
	{
		m_UniformData.try_emplace(pair.first);
	}
}

void Material::Use()
{
	m_Shader->Use();

	for (auto& [name, uniform] : m_UniformData)
	{
		m_Shader->SetUniform(name, uniform);
	}	
}
