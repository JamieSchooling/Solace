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
	void Initialise(entt::registry& r, entt::entity e) override;
	void DrawInspector(entt::registry& r, entt::entity e) override;
private:
	IProperty* m_projTypeProp;
	IProperty* m_fovProp;
	IProperty* m_sizeProp;
	IProperty* m_nearProp;
	IProperty* m_farProp;
};
CUSTOM_INSPECTOR(Camera, CameraInspector)