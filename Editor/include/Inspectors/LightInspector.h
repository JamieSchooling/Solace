#pragma once

#include "ComponentInspector.h"
#include "InspectorRegistry.h"

#include <Rendering/Light.h>

#include <iostream>

class LightInspector : public ComponentInspector
{
	using ComponentInspector::ComponentInspector;
protected:
	void DrawInspector(entt::registry& r, entt::entity e) override;
};
CUSTOM_INSPECTOR(Light, LightInspector)