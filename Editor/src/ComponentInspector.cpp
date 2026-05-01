#include "ComponentInspector.h"

ComponentInspector::ComponentInspector(std::string title) : m_Title(title)
{
}

void ComponentInspector::Draw()
{
	ImGui::Separator();
	ImGui::TextUnformatted("Transform");
	for (auto& property : m_Properties)
	{
		property->Draw();
	}
	ImGui::Separator();
}
