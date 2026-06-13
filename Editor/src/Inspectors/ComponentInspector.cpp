#include "Inspectors/ComponentInspector.h"

#include "Inspectors/EditorProperty.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <magic_enum/magic_enum.hpp>

ComponentInspector::ComponentInspector(std::shared_ptr<IComponentReflection> component)
	: m_component(component)
{
}

void ComponentInspector::Draw(entt::registry& r, entt::entity e)
{
	ImGui::Separator();
	ImGui::TextUnformatted(m_component->Name());
	ImGui::PushID(m_component->Name());
	ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);
	if (ImGui::BeginPopupContextItem("##ComponentMenu"))
	{
		bool removed = false;
		if (ImGui::MenuItem("Remove Component"))
		{
			m_component->Erase(r, e);
			removed = true;
		}
		ImGui::EndPopup();
		if (removed) 
		{ 
			ImGui::PopID();
			EditorSystem::Get().SetSceneDirty();
			return; 
		}
	}
	if (ImGui::SmallButton("="))
	{
		ImGui::OpenPopup("##ComponentMenu");
	}
	DrawInspector(r, e);
	ImGui::PopID();
	ImGui::Separator();
}

void ComponentInspector::DrawInspector(entt::registry& r, entt::entity e)
{
	for (auto& property : m_component->GetProperties())
	{
		if (property->Type() == PropertyType::Bool)
		{
			bool value = std::any_cast<bool>(property->Get(r, e));
			if (EditorProperty<bool>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Int)
		{
			int value = std::any_cast<int>(property->Get(r, e));
			if (EditorProperty<int>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Float)
		{
			float value = std::any_cast<float>(property->Get(r, e));
			if (EditorProperty<float>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec2)
		{
			glm::vec2 value = std::any_cast<glm::vec2>(property->Get(r, e));
			if (EditorProperty<glm::vec2>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec3)
		{
			glm::vec3 value = std::any_cast<glm::vec3>(property->Get(r, e));
			if (EditorProperty<glm::vec3>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec4)
		{
			glm::vec4 value = std::any_cast<glm::vec4>(property->Get(r, e));
			if (EditorProperty<glm::vec4>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Quaternion)
		{
			glm::quat value = std::any_cast<glm::quat>(property->Get(r, e));
			if (EditorProperty<glm::quat>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::String)
		{
			std::string value = std::any_cast<std::string>(property->Get(r, e));
			if (EditorProperty<std::string>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Colour)
		{
			Colour value = std::any_cast<Colour>(property->Get(r, e));
			if (EditorProperty<Colour>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Asset)
		{
			AssetHandle value = std::any_cast<AssetHandle>(property->Get(r, e));
			if (EditorProperty<AssetHandle>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Enum)
		{
			EnumInfo value = std::any_cast<EnumInfo>(property->Get(r, e));
			if (EditorProperty<EnumInfo>(property->Name(), value).Draw())
			{
				property->Set(value.CurrentValue, r, e);
			}
		}
	}
}
