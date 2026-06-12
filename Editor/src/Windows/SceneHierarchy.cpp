#include "Windows/SceneHierarchy.h"

#include <imgui.h>
#include <Scenes/NameComponent.h>
#include <Transform/Transform.h>

#include "Core/EditorSystem.h"
#include <Scenes/OrderComponent.h>

void SceneHierarchy::Open()
{
	EditorSystem::Get().OpenWindow<SceneHierarchy>();
}

void SceneHierarchy::DrawContent(entt::entity& selected, Scene& scene)
{
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create Entity"))
		{
			entt::entity entity = scene.Registry.create();
			scene.Registry.emplace<NameComponent>(entity).Name = "Entity";
			scene.Registry.emplace<Transform>(entity);
			selected = entity;
		}
		ImGui::EndPopup();
	}

	ImGuiTreeNodeFlags baseNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx(scene.Name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
	{
		auto view = scene.Registry.view<entt::entity>();

		std::vector<entt::entity> entities(view.begin(), view.end());

		std::sort(entities.begin(), entities.end(), [&](entt::entity a, entt::entity b)
		{
			return scene.Registry.get<OrderComponent>(a).Order < scene.Registry.get<OrderComponent>(b).Order;
		});

		for (auto entity : entities)
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
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete"))
				{
					if (selected == entity) { selected = entt::null; }
					scene.Registry.destroy(entity);
				}
				ImGui::EndPopup();
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
