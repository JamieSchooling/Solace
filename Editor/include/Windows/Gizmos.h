#pragma once

#include <ImGuizmo.h>

struct Gizmos
{
	static inline ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	static inline ImGuizmo::MODE CurrentGizmoMode = ImGuizmo::WORLD;
	static inline bool WasManipulatedThisFrame = false;
};