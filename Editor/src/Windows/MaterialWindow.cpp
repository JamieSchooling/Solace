#include "Windows/MaterialWindow.h"

#include "Core/EditorSystem.h"
#include <Assets/MaterialSerialiser.h>
#include <Assets/MaterialAssetCache.h>

void MaterialWindow::Open()
{
	EditorSystem::Get().OpenWindow<MaterialWindow>();
}

void MaterialWindow::Open(std::filesystem::path materialPath)
{
	auto& window = EditorSystem::Get().OpenWindow<MaterialWindow>();
	window.m_materialPath = materialPath;
	window.m_material = MaterialAssetCache::Load(materialPath);
}

void MaterialWindow::DrawContent(entt::entity& selected, Scene& scene)
{
	for (auto [name, data] : m_material->GetUniformData())
	{
		bool isProperty = name.rfind("u_prop_", 0) == 0;
		if (!isProperty) { continue; }

		UniformDescription desc = m_material->GetUniformDescription(name);
		DrawProperty(name, data, desc);
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
	{
		MaterialSerialiser ms;
		ms.SerialiseTo(m_material, m_materialPath);
		ShowUnsaved(false);
	}
}

void MaterialWindow::DrawProperty(const std::string& name, UniformData data, UniformDescription desc)
{
	std::string prefix = "u_prop_";
	std::string displayName = name.substr(prefix.length());
	if (desc.type == ShaderDataType::Bool)
	{
		bool value = std::get<bool>(data);
		if (EditorProperty<bool>(displayName, value).Draw())
		{
			m_material->SetValue(name, value);
			ShowUnsaved();
		}
	}
	else if (desc.type == ShaderDataType::Int)
	{
		int value = std::get<int>(data);
		if (EditorProperty<int>(displayName, value).Draw())
		{
			m_material->SetValue(name, value);
			ShowUnsaved();
		}
	}
	else if (desc.type == ShaderDataType::Float)
	{
		float value = std::get<float>(data);
		if (EditorProperty<float>(displayName, value).Draw())
		{
			m_material->SetValue(name, value);
			ShowUnsaved();
		}
	}
	else if (desc.type == ShaderDataType::Vector2)
	{
		glm::vec2 value = std::get<glm::vec2>(data);
		if (EditorProperty<glm::vec2>(displayName, value).Draw())
		{
			m_material->SetValue(name, value);
			ShowUnsaved();
		}
	}
	else if (desc.type == ShaderDataType::Vector3)
	{
		bool isColour = displayName.find("colour") != std::string::npos || displayName.find("Colour") != std::string::npos;
		glm::vec3 value = std::get<glm::vec3>(data);
		if (isColour)
		{
			Colour c;
			c.ColourValue = glm::vec4(value.r, value.g, value.b, 1.0f);
			if (EditorProperty<Colour>(displayName, c).Draw())
			{
				m_material->SetValue(name, glm::vec3(c.ColourValue));
				ShowUnsaved();
			}
		}
		else
		{
			if (EditorProperty<glm::vec3>(displayName, value).Draw())
			{
				m_material->SetValue(name, value);
				ShowUnsaved();
			}
		}
	}
	else if (desc.type == ShaderDataType::Vector4)
	{
		bool isColour = displayName.find("colour") != std::string::npos || displayName.find("Colour") != std::string::npos;
		glm::vec4 value = std::get<glm::vec4>(data);
		if (isColour)
		{
			Colour c;
			c.ColourValue = value;
			if (EditorProperty<Colour>(displayName, c).Draw())
			{
				m_material->SetValue(name, c.ColourValue);
				ShowUnsaved();
			}

		}
		else
		{
			if (EditorProperty<glm::vec4>(displayName, value).Draw())
			{
				m_material->SetValue(name, value);
				ShowUnsaved();
			}
		}
	}
}
