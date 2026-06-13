#pragma once

#include <entt/entt.hpp>

#include <Scenes/Scene.h>

#include <Events/Event.h>

class EditorWindow
{
public:
	EditorWindow(const char* title) : m_title(title) {}

	virtual void Initialise(Scene& scene) {}
	virtual void Initialise() {}
	virtual void OnEvent(Event& e) {}
	void Draw(entt::entity& selected, Scene& scene);
	virtual void DrawGizmos(entt::entity& selected, Camera& editorCamera, Transform& editorCamTransform, Scene& scene) {}
protected:
	virtual void DrawContent(entt::entity& selected, Scene& scene) = 0;
	void ShowUnsaved(bool isUnsaved = true) { m_showUnsaved = isUnsaved; }
private:
	const char* m_title;
	bool m_showUnsaved = false;
	bool m_wasFocused = false;
};