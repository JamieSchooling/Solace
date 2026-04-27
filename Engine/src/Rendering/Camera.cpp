#include "Rendering/Camera.h"

Camera::Camera()
{
	RecalculateProjection();
}

void Camera::RecalculateProjection()
{
	m_Projection = [&]() {
		switch (ProjectionType)
		{
			case CameraProjectionType::Perspective: return glm::perspective(FOV, Window::Get().GetAspectRatio(), Near, Far);
			case CameraProjectionType::Orthographic: return glm::ortho(0.0f, Window::Get().GetWidth(), 0.0f, Window::Get().GetHeight(), Near, Far);
		}
	}();
}

const glm::mat4& Camera::GetProjection() const
{
	return m_Projection;
}
