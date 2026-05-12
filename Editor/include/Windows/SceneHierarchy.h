#pragma once

#include "Windows/EditorWindow.h"
#include "Menu/MenuRegistry.h"

class SceneHierarchy : public EditorWindow
{
public:
	SceneHierarchy() : EditorWindow("Hierarchy") {}

	static void Open();
	MENU_ITEM("Window/General/Hierarchy", 410, Open);
protected:
	void DrawContent(entt::entity& selected, Scene& scene) override;
};