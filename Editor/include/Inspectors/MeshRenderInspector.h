#pragma once

#include "ComponentInspector.h"
#include "InspectorRegistry.h"

#include <Rendering/MeshRender.h>
#include <Reflection/TypeID.h>

#include <iostream>

class MeshRenderInspector : public ComponentInspector
{
	using ComponentInspector::ComponentInspector;
protected:
	void DrawInspector(entt::registry& r, entt::entity e) override;
};
CUSTOM_INSPECTOR(MeshRenderComponent, MeshRenderInspector)