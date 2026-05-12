#pragma once

#include "Windows/EditorWindow.h"
#include "Menu/MenuRegistry.h"
#include "Inspectors/ComponentInspector.h"

class InspectorWindow : public EditorWindow
{
public:
	InspectorWindow() : EditorWindow("Inspector") {}

	static void Open();
	MENU_ITEM("Window/General/Inspector", 400, Open);

	void Initialise(Scene& scene) override;
protected:
	void DrawContent(entt::entity& selected, Scene& scene) override;
private:
	std::unordered_map<entt::entity, std::vector<std::shared_ptr<ComponentInspector>>> m_inspectors;
};