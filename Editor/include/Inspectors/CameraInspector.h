#pragma once

#include "ComponentInspector.h"
#include "InspectorRegistry.h"

#include <Rendering/Camera.h>
#include <Reflection/TypeID.h>

#include <iostream>

class CameraInspector : public ComponentInspector
{
	using ComponentInspector::ComponentInspector;
protected:
	void DrawInspector(entt::registry& r, entt::entity e) override;
};
CUSTOM_INSPECTOR(Camera, CameraInspector)