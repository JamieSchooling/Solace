#pragma once
#include <memory>
#include <Rendering/FBO.h>
#include <imgui.h>

#include "Windows/EditorWindow.h"
#include "Menu/MenuRegistry.h"

#include "Windows/Gizmos.h"


class SceneViewport : public EditorWindow
{
public:
	SceneViewport() : EditorWindow("Scene") {}

	static void Open();
	MENU_ITEM("Window/General/Scene Viewport", 440, Open, "");

	virtual void Initialise() override;
protected:
	void DrawContent(entt::entity& selected, Scene& scene) override;
	void OnEvent(Event& e) override;
private:
	glm::vec3 m_editorCamRotation;
	float m_editorFlyCamSpeed = 2.5f;
	bool m_capturingViewport = false;
	bool m_wasUsingGizmo = false;

	void DrawGizmosToViewport(entt::entity& selected, Scene& scene);
};