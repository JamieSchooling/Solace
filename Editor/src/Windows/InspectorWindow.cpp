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
		}
	}
}

void InspectorWindow::DrawContent(entt::entity& selected, Scene& scene)
{
	if (selected == entt::null)
	{
		return;
	}

	ImGui::PushID((int)selected);
	if (scene.Registry.all_of<NameComponent>(selected))
	{
		EditorProperty<std::string>("Name", scene.Registry.get<NameComponent>(selected).Name).Draw();
	}
	for (auto& inspector : m_inspectors[selected])
	{
		inspector->Draw(scene.Registry, selected);
	}
	ImGui::PopID();
}