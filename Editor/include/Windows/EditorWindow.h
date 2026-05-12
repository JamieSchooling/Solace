#pragma once

#include <entt/entt.hpp>

#include <Scenes/Scene.h>

class EditorWindow
{
public:
	EditorWindow(const char* title) : m_title(title) {}

	virtual void Initialise(Scene& scene) {}
	void Draw(entt::entity& selected, Scene& scene);
protected:
	virtual void DrawContent(entt::entity& selected, Scene& scene) = 0;
private:
	const char* m_title;
};