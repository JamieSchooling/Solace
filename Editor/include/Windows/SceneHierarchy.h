#pragma once

#include "Windows/EditorWindow.h"

class SceneHierarchy : public EditorWindow
{
public:
	SceneHierarchy() : EditorWindow("Hierarchy") {}
protected:
	void DrawContent(entt::entity& selected, Scene& scene);
};