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

	EditResult result;

	std::string prefix = "u_prop_";
	std::string displayName = name.substr(prefix.length());
	if (desc.type == ShaderDataType::Bool)
	{
		bool value = std::get<bool>(data);
		result = EditorProperty<bool>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
			ShowUnsaved();
		}
		if (result.EditStarted)
		{
			UndoSystem::BeginValueEdit(value);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndValueEdit(value, 
			[name, material](std::any before) {
				material->SetValue(name, std::any_cast<bool>(before));
			}, 
			[name, material](std::any after) {
				material->SetValue(name, std::any_cast<bool>(after));
			});
		}
	}
	else if (desc.type == ShaderDataType::Int)
	{
		int value = std::get<int>(data);
		result = EditorProperty<int>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
			ShowUnsaved();
		}
		if (result.EditStarted)
		{
			UndoSystem::BeginValueEdit(value);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndValueEdit(value,
			[name, material](std::any before)
			{
				material->SetValue(name, std::any_cast<int>(before));
			},
			[name, material](std::any after)
			{
				material->SetValue(name, std::any_cast<int>(after));
			});
		}
	}
	else if (desc.type == ShaderDataType::Float)
	{
		float value = std::get<float>(data);
		result = EditorProperty<float>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
			ShowUnsaved();
		}
		if (result.EditStarted)
		{
			UndoSystem::BeginValueEdit(value);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndValueEdit(value,
			[name, material](std::any before)
			{
				material->SetValue(name, std::any_cast<float>(before));
			},
			[name, material](std::any after)
			{
				material->SetValue(name, std::any_cast<float>(after));
			});
		}
	}
	else if (desc.type == ShaderDataType::Vector2)
	{
		glm::vec2 value = std::get<glm::vec2>(data);
		result = EditorProperty<glm::vec2>(displayName, value).Draw();
		if (result.Changed)
		{
			material->SetValue(name, value);
			ShowUnsaved();
		}
		if (result.EditStarted)
		{
			UndoSystem::BeginValueEdit(value);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndValueEdit(value,
			[name, material](std::any before)
			{
				material->SetValue(name, std::any_cast<glm::vec2>(before));
			},
			[name, material](std::any after)
			{
				material->SetValue(name, std::any_cast<glm::vec2>(after));
			});
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
			result = EditorProperty<Colour>(displayName, c).Draw();
			if (result.Changed)
			{
				material->SetValue(name, glm::vec3(c.ColourValue));
				ShowUnsaved();
			}
			if (result.EditStarted)
			{
				UndoSystem::BeginValueEdit(c.ColourValue);
			}
			if (result.EditEnded)
			{
				UndoSystem::EndValueEdit(c.ColourValue,
				[name, material](std::any before)
				{
					material->SetValue(name, glm::vec3(std::any_cast<glm::vec4>(before)));
				},
				[name, material](std::any after)
				{
					material->SetValue(name, glm::vec3(std::any_cast<glm::vec4>(after)));
				});
			}
		}
		else
		{
			result = EditorProperty<glm::vec3>(displayName, value).Draw();
			if (result.Changed)
			{
				material->SetValue(name, value);
				ShowUnsaved();
			}
			if (result.EditStarted)
			{
				UndoSystem::BeginValueEdit(value);
			}
			if (result.EditEnded)
			{
				UndoSystem::EndValueEdit(value,
				[name, material](std::any before)
				{
					material->SetValue(name, std::any_cast<glm::vec3>(before));
				},
				[name, material](std::any after)
				{
					material->SetValue(name, std::any_cast<glm::vec3>(after));
				});
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
			result = EditorProperty<Colour>(displayName, c).Draw();
			if (result.Changed)
			{
				material->SetValue(name, c.ColourValue);
				ShowUnsaved();
			}
			if (result.EditStarted)
			{
				UndoSystem::BeginValueEdit(c.ColourValue);
			}
			if (result.EditEnded)
			{
				UndoSystem::EndValueEdit(c.ColourValue,
				[name, material](std::any before)
				{
					material->SetValue(name, std::any_cast<glm::vec4>(before));
				},
				[name, material](std::any after)
				{
					material->SetValue(name, std::any_cast<glm::vec4>(after));
				});
			}

		}
		else
		{
			result = EditorProperty<glm::vec4>(displayName, value).Draw();
			if (result.Changed)
			{
				material->SetValue(name, value);
				ShowUnsaved();
			}
			if (result.EditStarted)
			{
				UndoSystem::BeginValueEdit(value);
			}
			if (result.EditEnded)
			{
				UndoSystem::EndValueEdit(value,
				[name, material](std::any before)
				{
					material->SetValue(name, std::any_cast<glm::vec4>(before));
				},
				[name, material](std::any after)
				{
					material->SetValue(name, std::any_cast<glm::vec4>(after));
				});
			}
		}
	}
	else if (desc.type == ShaderDataType::Texture2D)
	{
		AssetHandle value = std::get<AssetHandle>(data);
		result = EditorProperty<AssetHandle>(displayName, value).Draw();
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
					UndoSystem::BeginValueEdit(value);

					material->SetValue(name, handle);

					UndoSystem::EndValueEdit(handle,
					[name, material](std::any before)
					{
						material->SetValue(name, std::any_cast<AssetHandle>(before));
					},
					[name, material](std::any after)
					{
						material->SetValue(name, std::any_cast<AssetHandle>(after));
					});
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}
