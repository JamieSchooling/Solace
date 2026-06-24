#pragma once

#include "ComponentInspector.h"
#include "InspectorRegistry.h"

#include <Rendering/Light.h>

#include <iostream>

class LightInspector : public ComponentInspector
{
	using ComponentInspector::ComponentInspector;
protected:
	void Initialise(entt::registry& r, entt::entity e) override;
	void DrawInspector(entt::registry& r, entt::entity e) override;
private:
	IProperty* m_typeProp;
	IProperty* m_colourProp;
	IProperty* m_intensityProp;
	IProperty* m_radiusProp;
};
CUSTOM_INSPECTOR(Light, LightInspector)