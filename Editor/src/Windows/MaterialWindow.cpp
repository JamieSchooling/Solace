#include "Windows/MaterialWindow.h"
#include "Inspectors/EditorProperty.h"

#include "Core/EditorSystem.h"
#include <Assets/MaterialSerialiser.h>
#include <Assets/AssetCache.h>

void MaterialWindow::Open()
{
	EditorSystem::Get().OpenWindow<MaterialWindow>();
}

void MaterialWindow::Open(AssetHandle materialHandle)
{
	auto& window = EditorSystem::Get().OpenWindow<MaterialWindow>();
	window.m_materialPath = AssetRegistry::Get().GetFullPath(materialHandle);
	window.m_material = AssetCache::Load<Material>(materialHandle);
}

void MaterialWindow::DrawContent(entt::entity& selected, Scene& scene)
{
	auto material = m_material.lock();
	if (!material)
	{
		ShowUnsaved(false);
		return;
	}

	for (auto [name, data] : material->GetUniformData())
	{
		bool isProperty = name.rfind("u_prop_", 0) == 0;
		if (!isProperty) { continue; }

		UniformDescription desc = material->GetUniformDescription(name);
		DrawProperty(name, data, desc);
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteAlways))
	{
		MaterialSerialiser ms;
		ms.SerialiseTo(material, m_materialPath);
		ShowUnsaved(false);
	}
}

void MaterialWindow::OnEvent(Event& e)
{
	if (e.Type == EventType::WindowFocusLost)
	{
		if (auto material = m_material.lock())
		{
			MaterialSerialiser ms;
			ms.SerialiseTo(material, m_materialPath);
			ShowUnsaved(false);
		}
	}
}

void MaterialWindow::DrawProperty(const std::string& name, UniformData data, UniformDescription desc)
{
	auto material = m_material.lock();
	if (!material) { return; }

	std::string prefix = "u_prop_";
	std::string displayName = name.substr(prefix.length());
	if (desc.type == ShaderDataType::Bool)
	{
		bool value = std::get<bool>(data);
		EditResult result = EditorProperty<bool>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
			ShowUnsaved();
		}
	}
	else if (desc.type == ShaderDataType::Int)
	{
		int value = std::get<int>(data);
		EditResult result = EditorProperty<int>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
			ShowUnsaved();
		}
	}
	else if (desc.type == ShaderDataType::Float)
	{
		float value = std::get<float>(data);
		EditResult result = EditorProperty<float>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
			ShowUnsaved();
		}
	}
	else if (desc.type == ShaderDataType::Vector2)
	{
		glm::vec2 value = std::get<glm::vec2>(data);
		EditResult result = EditorProperty<glm::vec2>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
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
			EditResult result = EditorProperty<Colour>(displayName, c).Draw();
			if (result.Changed)
			{
				material->SetValue(name, glm::vec3(c.ColourValue));
				ShowUnsaved();
			}
		}
		else
		{
			EditResult result = EditorProperty<glm::vec3>(displayName, value).Draw();
			if (result.Changed)
			{
				material->SetValue(name, value);
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
			EditResult result = EditorProperty<Colour>(displayName, c).Draw();
			if (result.Changed)
			{
				material->SetValue(name, c.ColourValue);
				ShowUnsaved();
			}

		}
		else
		{
			EditResult result = EditorProperty<glm::vec4>(displayName, value).Draw();
			if (result.Changed)
			{
				material->SetValue(name, value);
				ShowUnsaved();
			}
		}
	}
	else if (desc.type == ShaderDataType::Texture2D)
	{
		AssetHandle value = std::get<AssetHandle>(data);
		EditResult result = EditorProperty<AssetHandle>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
			ShowUnsaved();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset_Item"))
			{
				AssetHandle handle = *(AssetHandle*)payload->Data;
				std::filesystem::path path = AssetRegistry::Get().GetFullPath(handle);
				if (Texture::IsImageFile(path))
				{
					material->SetValue(name, handle);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}
