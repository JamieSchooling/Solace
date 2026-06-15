#include "Windows/InspectorWindow.h"

#include <imgui.h>
#include <Scenes/NameComponent.h>
#include <Inspectors/EditorProperty.h>
#include <Reflection/ReflectionRegistry.h>
#include "Core/EditorSystem.h"

void InspectorWindow::Open()
{
	EditorSystem::Get().OpenWindow<InspectorWindow>();
}

void InspectorWindow::Initialise(Scene& scene)
{
	m_inspectors.clear();
	auto& registry = scene.Registry;
	for (auto& entity : registry.view<entt::entity>())
	{
		CacheEntityInspectors(entity, registry);
	}
}

void InspectorWindow::DrawGizmos(entt::entity& selected, Scene& scene)
{
	for (auto& inspector : m_inspectors[selected])
	{
		inspector->DrawGizmos(scene.Registry, selected);
	}
}

void InspectorWindow::EndFrame(entt::entity& selected, Scene& scene)
{
	for (auto& inspector : m_inspectors[selected])
	{
		inspector->EndFrame(scene.Registry, selected);
	}
}

void InspectorWindow::DrawContent(entt::entity& selected, Scene& scene)
{
	CacheEntityInspectors(selected, scene.Registry); // TODO: Implement check for component addition rather than this every frame

	if (selected == entt::null)
	{
		return;
	}

	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::BeginMenu("Add Component"))
		{
			auto componentReflections = ReflectionRegistry::Get();
			for (auto& component : componentReflections)
			{
				if (ImGui::MenuItem(component->Name()))
				{
					component->Emplace(scene.Registry, selected);
					component->Initialise(scene.Registry, selected);
					CacheEntityInspectors(selected, scene.Registry);
					EditorSystem::Get().SetSceneDirty();

					UndoCommand undo;
					undo.Action = [this, component, selected, &scene]()
					{
						component->Emplace(scene.Registry, selected);
						component->Initialise(scene.Registry, selected);
						CacheEntityInspectors(selected, scene.Registry);
						EditorSystem::Get().SetSceneDirty();
					};
					undo.UndoAction = [this, component, selected, &scene]()
					{
						component->Erase(scene.Registry, selected);
						CacheEntityInspectors(selected, scene.Registry);
						EditorSystem::Get().SetSceneDirty();
					};
					UndoSystem::AddUndoCommand(undo);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	if (!m_inspectors.contains(selected))
	{
		CacheEntityInspectors(selected, scene.Registry);
	}

	ImGui::PushID((int)selected);
	if (scene.Registry.all_of<NameComponent>(selected))
	{
		EditorProperty<std::string>("Name", scene.Registry.get<NameComponent>(selected).Name).Draw();
	}
	for (auto& inspector : m_inspectors[selected])
	{
		if (!inspector->GetComponent()->IsOnEntity(scene.Registry, selected))
		{
			auto it = std::find(m_inspectors.at(selected).begin(), m_inspectors.at(selected).end(), inspector);
			if (it != m_inspectors.at(selected).end())
			{
				m_inspectors.at(selected).erase(it);
			}
			continue;
		}
		inspector->Draw(scene.Registry, selected);
	}
	ImGui::PopID();
}

void InspectorWindow::CacheEntityInspectors(entt::entity entity, entt::registry& registry)
{
	m_inspectors[entity] = std::vector<std::shared_ptr<ComponentInspector>>();

	auto componentReflections = ReflectionRegistry::View(registry, entity);
	for (auto& component : componentReflections)
	{
		if (InspectorRegistry::Get().contains(component->GetTypeID()))
		{
			auto inspectorConstructor = InspectorRegistry::Get().at(component->GetTypeID());
			m_inspectors[entity].push_back(inspectorConstructor(component));
		}
		else
		{
			m_inspectors[entity].push_back(std::make_shared<ComponentInspector>(component));
		}
		m_inspectors[entity].back()->Initialise(registry, entity);
	}
}
