#pragma once

#include "Windows/EditorWindow.h"
#include "Menu/MenuRegistry.h"

#include <imgui.h>

class SceneHierarchy : public EditorWindow
{
public:
	SceneHierarchy() : EditorWindow("Hierarchy") {}

	static void Open();
	MENU_ITEM("Window/General/Hierarchy", 410, Open);
protected:
	void DrawContent(entt::entity& selected, Scene& scene) override;

private:
	void DrawEntity(Scene& scene, entt::entity entity, entt::entity& selected, ImGuiTreeNodeFlags flags);
	void DrawDropSlot(Scene& scene, size_t insertIndex);
};