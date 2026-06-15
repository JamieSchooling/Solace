#include "Windows/SceneHierarchy.h"

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
	ShowUnsaved(EditorSystem::Get().IsSceneDirty());

	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create Entity"))
		{
			entt::entity entity = scene.Registry.create();
			scene.Registry.emplace<NameComponent>(entity).Name = "Entity";
			scene.Registry.emplace<OrderComponent>(entity).Order = scene.Registry.view<entt::entity>().size();
			scene.Registry.emplace<Transform>(entity);
			selected = entity;
			EditorSystem::Get().SetSceneDirty();

			UndoCommand undo;
			undo.Action = [this, &selected, &scene, entity]()
			{
				entt::entity restored = scene.Registry.create(entity);
				scene.Registry.emplace<NameComponent>(restored).Name = "Entity";
				scene.Registry.emplace<OrderComponent>(restored).Order = scene.Registry.view<entt::entity>().size();
				scene.Registry.emplace<Transform>(restored);
				selected = restored;
				EditorSystem::Get().SetSceneDirty();
			};
			undo.UndoAction = [this, entity, &scene, &selected]()
			{
				if (selected == entity) { selected = entt::null; }
				scene.Registry.destroy(entity);
				EditorSystem::Get().SetSceneDirty();
			};
			UndoSystem::AddUndoCommand(undo);
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

		size_t pos = 0;
		for (auto entity : entities)
		{
			DrawDropSlot(scene, pos);
			DrawEntity(scene, entity, selected, baseNodeFlags);
			pos++;
		}
		DrawDropSlot(scene, pos);
		ImGui::TreePop();
	}

	// Check for deselection
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
	{
		selected = entt::null;
	}
}

void SceneHierarchy::DrawEntity(Scene& scene, entt::entity entity, entt::entity& selected, ImGuiTreeNodeFlags flags)
{
	std::string name = "Unnamed Entity";
	if (scene.Registry.all_of<NameComponent>(entity))
	{
		name = scene.Registry.get<NameComponent>(entity).Name;
	}
	ImGui::PushID((int)entity);
	ImGuiTreeNodeFlags nodeFlags = flags;
	if (entity == selected)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	if (ImGui::TreeNodeEx(name.c_str(), nodeFlags))
	{
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			selected = entity;
		}
		ImGui::TreePop();
	}
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("SCENE_ENTITY", &entity, sizeof(entt::entity), ImGuiCond_Once);
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete"))
		{
			using ComponentValues = std::vector<std::pair<std::string, std::any>>;
			std::vector<std::pair<std::string, ComponentValues>> entityComponents;
			for (auto component : ReflectionRegistry::View(scene.Registry, entity))
			{
				ComponentValues componentValues;
				for (auto property : component->GetProperties())
				{
					std::any value = property->Get(scene.Registry, entity);
					if (property->Type() == PropertyType::Enum)
					{
						EnumInfo info = std::any_cast<EnumInfo>(value);
						value = info.CurrentValue;
					}
					componentValues.push_back({ property->Name(), value });
				}
				entityComponents.push_back({ component->Name(), componentValues });
			}
			std::string entityName = scene.Registry.get<NameComponent>(entity).Name;
			int entityOrder = scene.Registry.get<OrderComponent>(entity).Order;
			

			if (selected == entity) { selected = entt::null; }
			scene.Registry.destroy(entity);
			EditorSystem::Get().SetSceneDirty();

			UndoCommand undo;
			undo.Action = [&selected, &scene, entity]()
			{
				if (selected == entity) { selected = entt::null; }
				scene.Registry.destroy(entity);
				EditorSystem::Get().SetSceneDirty();
			};
			undo.UndoAction = [&scene, entity, entityComponents, entityName, entityOrder]()
			{
				entt::entity restored = scene.Registry.create(entity);
				scene.Registry.emplace<NameComponent>(restored).Name = entityName;
				scene.Registry.emplace<OrderComponent>(restored).Order = entityOrder;

				for (auto [name, componentValues] : entityComponents)
				{
					auto component = ReflectionRegistry::Get(name.c_str());
					component->Emplace(scene.Registry, entity);
					for (auto& [name, value] : componentValues)
					{
						component->GetProperty(name.c_str())->Set(value, scene.Registry, entity);
					}
					component->Initialise(scene.Registry, entity);
				}
			};
			UndoSystem::AddUndoCommand(undo);
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}

void SceneHierarchy::DrawDropSlot(Scene& scene, size_t insertIndex)
{
	ImGui::PushID((int)insertIndex);

	ImGui::InvisibleButton("DropSlot", ImVec2(-1.0f, 2.0f));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_ENTITY"))
		{
			entt::entity dragged = *static_cast<entt::entity*>(payload->Data);

			size_t insertPos = insertIndex;

			auto view = scene.Registry.view<entt::entity>();

			std::vector<entt::entity> entities(view.begin(), view.end());

			std::sort(entities.begin(), entities.end(), [&](entt::entity a, entt::entity b)
			{
				return scene.Registry.get<OrderComponent>(a).Order < scene.Registry.get<OrderComponent>(b).Order;
			});

			auto it = std::find(entities.begin(), entities.end(), dragged);
			size_t oldIndex = scene.Registry.get<OrderComponent>(dragged).Order;

			entities.erase(it);

			if (insertPos > oldIndex)
				insertPos--;

			entities.insert(entities.begin() + insertPos, dragged);

			for (size_t i = 0; i < entities.size(); ++i)
			{
				scene.Registry.get<OrderComponent>(entities[i]).Order = i;
			}

			EditorSystem::Get().SetSceneDirty();
			UndoCommand undo;
			undo.Action = [dragged, insertIndex, &scene]()
			{
				size_t insertPos = insertIndex;

				auto view = scene.Registry.view<entt::entity>();

				std::vector<entt::entity> entities(view.begin(), view.end());

				std::sort(entities.begin(), entities.end(), [&](entt::entity a, entt::entity b)
				{
					return scene.Registry.get<OrderComponent>(a).Order < scene.Registry.get<OrderComponent>(b).Order;
				});

				auto it = std::find(entities.begin(), entities.end(), dragged);
				size_t oldIndex = scene.Registry.get<OrderComponent>(dragged).Order;

				entities.erase(it);

				if (insertPos > oldIndex)
					insertPos--;

				entities.insert(entities.begin() + insertPos, dragged);

				for (size_t i = 0; i < entities.size(); ++i)
				{
					scene.Registry.get<OrderComponent>(entities[i]).Order = i;
				}

				EditorSystem::Get().SetSceneDirty();
			};
			undo.UndoAction = [dragged, oldIndex, insertIndex, &scene]()
			{
				auto view = scene.Registry.view<entt::entity>();

				size_t insertPos = oldIndex;

				std::vector<entt::entity> entities(view.begin(), view.end());

				std::sort(entities.begin(), entities.end(), [&](entt::entity a, entt::entity b)
				{
					return scene.Registry.get<OrderComponent>(a).Order < scene.Registry.get<OrderComponent>(b).Order;
				});

				auto it = std::find(entities.begin(), entities.end(), dragged);
				size_t oldIndex = scene.Registry.get<OrderComponent>(dragged).Order;

				entities.erase(it);

				if (insertPos > insertIndex)
					insertPos--;

				entities.insert(entities.begin() + insertPos, dragged);

				for (size_t i = 0; i < entities.size(); ++i)
				{
					scene.Registry.get<OrderComponent>(entities[i]).Order = i;
				}
			};
			UndoSystem::AddUndoCommand(undo);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();
}
