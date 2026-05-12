#include "Windows/SceneHierarchy.h"

#include <imgui.h>
#include <Scenes/NameComponent.h>

#include "Core/EditorSystem.h"

void SceneHierarchy::Open()
{
	EditorSystem::Get().OpenWindow<SceneHierarchy>();
}

void SceneHierarchy::DrawContent(entt::entity& selected, Scene& scene)
{
	ImGuiTreeNodeFlags baseNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx(scene.Name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
	{
		for (auto& entity : scene.Registry.view<entt::entity>())
		{
			std::string m_name = "Unnamed Entity";
			if (scene.Registry.all_of<NameComponent>(entity))
			{
				m_name = scene.Registry.get<NameComponent>(entity).Name;
			}
			ImGui::PushID((int)entity);
			ImGuiTreeNodeFlags nodeFlags = baseNodeFlags;
			if (entity == selected)
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			if (ImGui::TreeNodeEx(m_name.c_str(), nodeFlags))
			{
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					selected = entity;
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	// Check for deselection
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
	{
		selected = entt::null;
	}
}
