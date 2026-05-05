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

static bool cInspectorReg = []()
{
	InspectorRegistry::Get()[Hash("Camera")] = &CustomInspector<CameraInspector>;
	return true;
}();