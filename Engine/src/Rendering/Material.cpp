#include "Rendering/Material.h"

Material::Material(std::shared_ptr<Shader> shader) : m_shader(shader)
{
	for (auto& pair : m_shader->m_uniformLookup)
	{
		m_uniformData.try_emplace(pair.first);
	}
}

void Material::Use()
{
	m_shader->Use();

	for (auto& [m_name, uniform] : m_uniformData)
	{
		m_shader->SetUniform(m_name, uniform);
	}	
}
