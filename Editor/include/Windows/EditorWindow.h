#pragma once

#include <entt/entt.hpp>

#include <Scenes/Scene.h>

#include <Events/Event.h>

class EditorWindow
{
public:
	EditorWindow(const char* title) : m_title(title) {}

	virtual void Initialise(Scene& scene) {}
	virtual void OnEvent(Event& e) {}
	void Draw(entt::entity& selected, Scene& scene);
protected:
	virtual void DrawContent(entt::entity& selected, Scene& scene) = 0;
private:
	const char* m_title;
};