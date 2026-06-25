#include "Inspectors/ComponentInspector.h"

#include "Inspectors/EditorProperty.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <magic_enum/magic_enum.hpp>
#include <Core/Fonts.h>

ComponentInspector::ComponentInspector(std::shared_ptr<IComponentReflection> component)
	: m_component(component)
{
}

void ComponentInspector::Draw(entt::registry& r, entt::entity e)
{
	ImGui::PushFont(Fonts::Bold);
	if (ImGui::TreeNodeEx(m_component->Name(), ImGuiTreeNodeFlags_DefaultOpen))
	{	
		ImGui::PopFont();
		DrawMenuButton(r, e);
		DrawInspector(r, e);
		ImGui::TreePop();
	}
	else
	{
		ImGui::PopFont();
		DrawMenuButton(r, e);
	}
}

void ComponentInspector::DrawInspector(entt::registry& r, entt::entity e)
{
	auto properties = m_component->GetProperties();
	for (size_t i = 0; i < properties.size(); ++i)
	{
		if (const char* header = m_component->TryGetHeader(i))
		{
			ImGui::PushFont(Fonts::Bold);
			ImGui::TextUnformatted(header);
			ImGui::PopFont();
		}
		auto& property = properties[i];
		EditResult result;
		if (property->Type() == PropertyType::Bool)
		{
			bool value = std::any_cast<bool>(property->Get(r, e));
			result = EditorProperty<bool>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Int)
		{
			int value = std::any_cast<int>(property->Get(r, e));
			result = EditorProperty<int>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Float)
		{
			float value = std::any_cast<float>(property->Get(r, e));
			result = EditorProperty<float>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec2)
		{
			glm::vec2 value = std::any_cast<glm::vec2>(property->Get(r, e));
			result = EditorProperty<glm::vec2>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec3)
		{
			glm::vec3 value = std::any_cast<glm::vec3>(property->Get(r, e));
			result = EditorProperty<glm::vec3>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec4)
		{
			glm::vec4 value = std::any_cast<glm::vec4>(property->Get(r, e));
			result = EditorProperty<glm::vec4>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Quaternion)
		{
			glm::quat value = std::any_cast<glm::quat>(property->Get(r, e));
			result = EditorProperty<glm::quat>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::String)
		{
			std::string value = std::any_cast<std::string>(property->Get(r, e));
			result = EditorProperty<std::string>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Colour)
		{
			Colour value = std::any_cast<Colour>(property->Get(r, e));
			result = EditorProperty<Colour>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Asset)
		{
			AssetHandle value = std::any_cast<AssetHandle>(property->Get(r, e));
			result = EditorProperty<AssetHandle>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Enum)
		{
			EnumInfo value = std::any_cast<EnumInfo>(property->Get(r, e));
			result = EditorProperty<EnumInfo>(property->Name(), value, property->Attributes()).Draw();
			if (result.Changed)
			{
				property->Set(value.CurrentValue, r, e);
			}
		}


		if (result.EditStarted)
		{
			UndoSystem::BeginPropertyEdit(property, r, e);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndPropertyEdit(r, e);
		}
	}
}

void ComponentInspector::DrawMenuButton(entt::registry& r, entt::entity e)
{
	ImGui::PushID(m_component->Name());
	ImGui::SameLine(ImGui::GetWindowWidth() - 40.f);
	if (ImGui::BeginPopupContextItem("##ComponentMenu"))
	{
		bool removed = false;
		if (ImGui::MenuItem("Remove Component"))
		{
			std::vector<std::pair<std::string, std::any>> componentValues;
			for (auto property : m_component->GetProperties())
			{
				std::any value = property->Get(r, e);
				if (property->Type() == PropertyType::Enum)
				{
					EnumInfo info = std::any_cast<EnumInfo>(value);
					value = info.CurrentValue;
				}
				componentValues.push_back({ property->Name(), value });
			}
			m_component->Erase(r, e);
			removed = true;

			UndoCommand undo;
			auto component = m_component;
			undo.Action = [component, &r, e]()
			{
				component->Erase(r, e);
			};
			undo.UndoAction = [component, &r, e, componentValues]()
			{
				component->Emplace(r, e);
				for (auto& [name, value] : componentValues)
				{
					component->GetProperty(name.c_str())->Set(value, r, e);
				}
				component->Initialise(r, e);
			};
			UndoSystem::AddUndoCommand(undo);
		}
		ImGui::EndPopup();
		if (removed)
		{
			ImGui::PopID();
			EditorSystem::Get().SetSceneDirty();
			return;
		}
	}
	ImGui::PushFont(nullptr, 14.0f);
	if (ImGui::SmallButton("..."))
	{
		ImGui::OpenPopup("##ComponentMenu");
	}
	ImGui::PopFont();
	ImGui::PopID();
}
