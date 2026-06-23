#pragma once

#include <glm/glm.hpp>
#include <format>
#include <string>
#include <iostream>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <Reflection/Property.h>

#include <Rendering/Colour.h>
#include "Core/EditorSystem.h"

#include <Reflection/PropertyAttributes.h>

template<typename T>
using PropertyCallback = std::function<void(T&)>;

struct EditResult
{
	bool Changed = false;
	bool EditStarted = false;
	bool EditEnded = false;
};

class IEditorProperty
{
public:
	virtual ~IEditorProperty() = default;
	virtual EditResult Draw() = 0;
};

struct AttributeInfo
{
	bool IsSlider = false;
	float Min = 0.0f;
	float Max = 0.0f;
};

template<typename T>
class EditorProperty : public IEditorProperty
{
public:
	EditorProperty(std::string label, T& data, const std::vector<std::shared_ptr<PropertyAttribute>> propertyAttributes = {}, PropertyCallback<T> onChange = nullptr)
		: m_label(label), m_data(data), m_attributes(propertyAttributes), m_onChange(onChange)
	{}

	EditResult Draw() override;
private:
	T& m_data;
	std::string m_label;
	std::vector<std::shared_ptr<PropertyAttribute>> m_attributes;
	PropertyCallback<T> m_onChange;

	bool DrawPropertyWidget();
	AttributeInfo GetAttributeInfo();
};

template<typename T>
inline EditResult EditorProperty<T>::Draw()
{
	EditResult result;
	if (ImGui::BeginTable(std::format("##{}Property", m_label).c_str(), 2))
	{

		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.25f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.75f);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(m_label.c_str());

		ImGui::TableSetColumnIndex(1);
		result.Changed = DrawPropertyWidget();
		if (ImGui::IsItemActivated())
		{
			result.EditStarted = true;
		}
		if constexpr (std::is_same_v<T, EnumInfo> /*|| std::is_same_v<T, Colour>*/) // TODO: Fix colour undo redo (check if edit ended is occurring and the values at the time if so)
		{
			if (result.Changed)
			{
				result.EditEnded = true;
			}
		}
		else
		{
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				result.EditEnded = true;
			}
		}

		if (result.Changed) { EditorSystem::Get().SetSceneDirty(); }
		if (result.Changed && m_onChange)
		{
			m_onChange(m_data);
		}

		ImVec2 endPos = ImGui::GetCursorScreenPos();

		ImGui::EndTable();

		return result;
	}

	return result;
}

template<typename T>
bool EditorProperty<T>::DrawPropertyWidget()
{
	std::string idStr = std::format("##{}", m_label);
	const char* id = idStr.c_str();

	AttributeInfo attrInfo = GetAttributeInfo();

	if constexpr (std::is_same_v<T, bool>)
	{
		return ImGui::Checkbox(id, &m_data);
	}
	else if constexpr (std::is_same_v<T, int>)
	{
		return ImGui::DragInt(id, &m_data);
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		return attrInfo.IsSlider ? ImGui::SliderFloat(id, &m_data, attrInfo.Min, attrInfo.Max) 
			: ImGui::DragFloat(id, &m_data, 1.0f, attrInfo.Min, attrInfo.Max);
	}
	else if constexpr (std::is_same_v<T, glm::vec2>)
	{
		return attrInfo.IsSlider ? ImGui::SliderFloat2(id, &m_data.x, attrInfo.Min, attrInfo.Max)
			: ImGui::DragFloat2(id, &m_data.x, 1.0f, attrInfo.Min, attrInfo.Max);
	}
	else if constexpr (std::is_same_v<T, glm::vec3>)
	{
		return attrInfo.IsSlider ? ImGui::SliderFloat3(id, &m_data.x, attrInfo.Min, attrInfo.Max)
			: ImGui::DragFloat3(id, &m_data.x, 1.0f, attrInfo.Min, attrInfo.Max);
	}
	else if constexpr (std::is_same_v<T, glm::vec4>)
	{
		return attrInfo.IsSlider ? ImGui::SliderFloat4(id, &m_data.x, attrInfo.Min, attrInfo.Max)
			: ImGui::DragFloat4(id, &m_data.x, 1.0f, attrInfo.Min, attrInfo.Max);
	}
	else if constexpr (std::is_same_v<T, glm::quat>)
	{
		return ImGui::DragFloat4(id, &m_data.x);
	}
	else if constexpr (std::is_same_v<T, std::string>)
	{
		return ImGui::InputText(id, &m_data);
	}
	else if constexpr (std::is_same_v<T, Colour>)
	{
		return ImGui::ColorEdit4(id, &m_data.ColourValue.r);
	}
	else if constexpr (std::is_same_v<T, AssetHandle>)
	{
		std::string pathString = AssetRegistry::Get().GetPath(m_data).stem().string();
		return ImGui::InputText(id, &pathString);
	}
	else if constexpr (std::is_same_v<T, EnumInfo>)
	{
		std::vector<const char*> names;

		int currentIndex = 0;

		for (int i = 0; i < m_data.Entries.size(); i++)
		{
			names.push_back(m_data.Entries[i].Name.c_str());

			if (m_data.Entries[i].Value == m_data.CurrentValue)
			{
				currentIndex = i;
			}
		}

		if (ImGui::Combo(id, &currentIndex, names.data(), (int)names.size()))
		{
			m_data.CurrentValue =
				m_data.Entries[currentIndex].Value;

			m_data.SetValue(m_data.CurrentValue);

			return true;
		}

		return false;
	}
	else
	{
		static_assert(sizeof(T) == 0, "Unsupported property type");
	}
}

template<typename T>
AttributeInfo EditorProperty<T>::GetAttributeInfo()
{
	AttributeInfo attrInfo;
	for (auto attribute : m_attributes)
	{
		if (auto range = std::dynamic_pointer_cast<RangeAttribute>(attribute))
		{
			attrInfo.IsSlider = true;
			attrInfo.Min = range->Min;
			attrInfo.Max = range->Max;
		}
		else if (auto minAttr = std::dynamic_pointer_cast<MinAttribute>(attribute))
		{
			attrInfo.Min = minAttr->MinValue;
			if (attrInfo.Max <= attrInfo.Min)
			{
				attrInfo.Max = std::numeric_limits<float>::max();
			}
		}
	}
	return attrInfo;
}
