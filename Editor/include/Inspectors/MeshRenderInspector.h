#pragma once

#include "ComponentInspector.h"
#include "InspectorRegistry.h"

#include <Rendering/MeshRender.h>
#include <Reflection/TypeID.h>

#include <filesystem>
#include <iostream>

class MeshRenderInspector : public ComponentInspector
{
	using ComponentInspector::ComponentInspector;
protected:
	void Initialise(entt::registry& r, entt::entity e) override;
	void DrawInspector(entt::registry& r, entt::entity e) override;
private:
	IProperty* m_meshProp;
	IProperty* m_materialProp;
};
CUSTOM_INSPECTOR(MeshRenderComponent, MeshRenderInspector)