#pragma once

#include <entt/entt.hpp>

#include <Scenes/Scene.h>

#include <Events/Event.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

class EditorWindow
{
public:
	EditorWindow(const char* title, ImGuiWindowFlags flags = ImGuiWindowFlags_None) : m_title(title), m_flags(flags) {}

	virtual void Initialise(Scene& scene) {}
	virtual void Initialise() {}
	virtual void OnEvent(Event& e) {}
	void Draw(entt::entity& selected, Scene& scene);
	void DispatchEvents();
	virtual void DrawGizmos(entt::entity& selected, Camera& editorCamera, Transform& editorCamTransform, Scene& scene) {}
protected:
	virtual void DrawContent(entt::entity& selected, Scene& scene) = 0;
	void ShowUnsaved(bool isUnsaved = true) { m_showUnsaved = isUnsaved; }
	ImVec2 m_windowSize;
private:
	const char* m_title;
	bool m_showUnsaved = false;
	bool m_wasFocused = false;
	ImVec2 m_lastFrameWindowSize;
	ImGuiWindowFlags m_flags;
	std::vector<Event> m_eventQueue;
};