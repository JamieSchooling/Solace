#pragma once
#include <memory>
#include <Rendering/FBO.h>
#include <imgui.h>

#include "Windows/EditorWindow.h"
#include "Menu/MenuRegistry.h"

#include "Windows/Gizmos.h"


class GameViewport : public EditorWindow
{
public:
	GameViewport() : EditorWindow("Game") {}

	static void Open();
	MENU_ITEM("Window/General/Game Viewport", 450, Open);
protected:
	void DrawContent(entt::entity& selected, Scene& scene) override;
	void OnEvent(Event& e) override;
};