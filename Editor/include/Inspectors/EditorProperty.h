#pragma once

#include <glm/glm.hpp>
#include <format>
#include <string>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

template<typename T>
using PropertyCallback = std::function<void(T&)>;

class IEditorProperty
{
public:
	virtual ~IEditorProperty() = default;
	virtual bool Draw(bool samelineAfterDraw = false) = 0;
};

template<typename T>
class EditorProperty : public IEditorProperty
{
public:
	EditorProperty(std::string label, T& data, PropertyCallback<T> onChange = nullptr)
		: m_label(label), m_data(data), m_onChange(onChange)
	{}

	bool Draw(bool samelineAfterDraw = false) override;
private:
	T& m_data;
	std::string m_label;
	PropertyCallback<T> m_onChange;

	bool DrawPropertyWidget();
};

template<typename T>
inline bool EditorProperty<T>::Draw(bool samelineAfterDraw)
{
	if (ImGui::BeginTable(std::format("##{}Property", m_label).c_str(), 2))
	{
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(m_label.c_str());

		ImGui::TableSetColumnIndex(1);
		bool changed = DrawPropertyWidget();
		if (changed && m_onChange)
		{
			m_onChange(m_data);
		}

		if (samelineAfterDraw)
		{
			ImGui::SameLine();
		}
		ImVec2 endPos = ImGui::GetCursorScreenPos();

		ImGui::EndTable();

		if (samelineAfterDraw)
		{
			ImGui::SetCursorScreenPos(endPos);
		}

		return changed;
	}

	return false;
}

template<typename T>
bool EditorProperty<T>::DrawPropertyWidget()
{
	std::string idStr = std::format("##{}", m_label);
	const char* id = idStr.c_str();

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
		return ImGui::DragFloat(id, &m_data);
	}
	else if constexpr (std::is_same_v<T, glm::vec2>)
	{
		return ImGui::DragFloat2(id, &m_data.x);
	}
	else if constexpr (std::is_same_v<T, glm::vec3>)
	{
		return ImGui::DragFloat3(id, &m_data.x);
	}
	else if constexpr (std::is_same_v<T, glm::vec4>)
	{
		return ImGui::DragFloat4(id, &m_data.x);
	}
	else if constexpr (std::is_same_v<T, glm::quat>)
	{
		return ImGui::DragFloat4(id, &m_data.x);
	}
	else if constexpr (std::is_same_v<T, std::string>)
	{
		return ImGui::InputText(id, &m_data);
	}
	else
	{
		static_assert(sizeof(T) == 0, "Unsupported property type");
	}
}
