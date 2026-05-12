#pragma once

#include "Windows/EditorWindow.h"
#include <Inspectors/ComponentInspector.h>

class InspectorWindow : public EditorWindow
{
public:
	InspectorWindow() : EditorWindow("Inspector") {}

	void Initialise(Scene& scene);
protected:
	void DrawContent(entt::entity& selected, Scene& scene);
private:
	std::unordered_map<entt::entity, std::vector<std::shared_ptr<ComponentInspector>>> m_inspectors;
};