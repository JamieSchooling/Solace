#pragma once

#include <glm/glm.hpp>
#include <format>
#include <string>

#include <imgui.h>

template<typename T>
using PropertyCallback = std::function<void(T&)>;

class IEditorProperty
{
public:
	virtual ~IEditorProperty() = default;
	virtual void Draw() = 0;
};

template<typename T>
class EditorProperty : public IEditorProperty
{
public:
	EditorProperty(std::string label, T& data, PropertyCallback<T> onChange = nullptr)
		: m_Label(label), m_Data(data), m_OnChange(onChange)
	{}

	void Draw() override;
private:
	T& m_Data;
	std::string m_Label;
	PropertyCallback<T> m_OnChange;

	bool DrawPropertyWidget();
};

template<typename T>
inline void EditorProperty<T>::Draw()
{
	if (ImGui::BeginTable(std::format("##{}Property", m_Label).c_str(), 2))
	{
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(m_Label.c_str());

		ImGui::TableSetColumnIndex(1);
		if (DrawPropertyWidget() && m_OnChange)
		{
			m_OnChange(m_Data);
		}

		ImGui::EndTable();
	}
}

template<typename T>
bool EditorProperty<T>::DrawPropertyWidget()
{
	std::string idStr = std::format("##{}", m_Label);
	const char* id = idStr.c_str();

	if constexpr (std::is_same_v<T, bool>)
	{
		return ImGui::Checkbox(id, &m_Data);
	}
	else if constexpr (std::is_same_v<T, int>)
	{
		return ImGui::DragInt(id, &m_Data);
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		return ImGui::DragFloat(id, &m_Data);
	}
	else if constexpr (std::is_same_v<T, glm::vec2>)
	{
		return ImGui::DragFloat2(id, &m_Data.x);
	}
	else if constexpr (std::is_same_v<T, glm::vec3>)
	{
		return ImGui::DragFloat3(id, &m_Data.x);
	}
	else if constexpr (std::is_same_v<T, glm::vec4>)
	{
		return ImGui::DragFloat4(id, &m_Data.x);
	}
	else
	{
		static_assert(sizeof(T) == 0, "Unsupported property type");
	}
}
