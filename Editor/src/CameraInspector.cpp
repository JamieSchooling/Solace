#include "CameraInspector.h"

CameraInspector::CameraInspector(Camera& camera) : m_Camera(camera), ComponentInspector("Camera")
{
	m_Properties.push_back(std::make_shared<EditorProperty<float>>("FOV", m_Camera.FOV, [this](float& v) { m_Camera.RecalculateProjection(); }));
	m_Properties.push_back(std::make_shared<EditorProperty<float>>("Near", m_Camera.Near, [this](float& v) { m_Camera.RecalculateProjection(); }));
	m_Properties.push_back(std::make_shared<EditorProperty<float>>("Far", m_Camera.Far, [this](float& v) { m_Camera.RecalculateProjection(); }));
}
