#include "Rendering/Material.h"

Material::Material(std::shared_ptr<Shader> shader, MaterialInitFlags flags) : m_shader(shader)
{
	for (auto& [name, desc] : m_shader->m_uniformLookup)
	{
		if ((flags & MaterialInitFlags::InitWithDefaultValues) != 0)
		{
			SetDefaultValue(name, desc);
		}
		m_uniformData.try_emplace(name);
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

void Material::SetDefaultValue(const std::string& name, UniformDescription desc)
{
	if (desc.type == ShaderDataType::Bool)
	{
		m_uniformData[name] = false;
	}
	else if (desc.type == ShaderDataType::Int)
	{
		m_uniformData[name] = 0;
	}
	else if (desc.type == ShaderDataType::Float)
	{
		m_uniformData[name] = 0.0f;
	}
	else if (desc.type == ShaderDataType::Vector2)
	{
		m_uniformData[name] = glm::vec2(0.0f);
	}
	else if (desc.type == ShaderDataType::Vector3)
	{
		m_uniformData[name] = glm::vec3(0.0f);
	}
	else if (desc.type == ShaderDataType::Vector4)
	{
		m_uniformData[name] = glm::vec4(0.0f);
	}
	else if (desc.type == ShaderDataType::Texture2D)
	{
		m_uniformData[name] = AssetHandle();
	}
}