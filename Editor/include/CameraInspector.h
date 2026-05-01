#pragma once

#include "ComponentInspector.h"

#include <Rendering/Camera.h>

class CameraInspector : public ComponentInspector
{
public:
	CameraInspector(Camera& camera);
private:
	Camera& m_Camera;
};