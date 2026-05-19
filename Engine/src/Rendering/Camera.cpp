#include "Rendering/Camera.h"

Camera::Camera()
{
	RecalculateProjection();
}

void Camera::RecalculateProjection()
{
	m_projection = [&]() {
		switch (ProjectionType)
		{
			case CameraProjectionType::Perspective: return glm::perspective(glm::radians(FOV), Window::Get().GetAspectRatio(), Near, Far);
			case CameraProjectionType::Orthographic: return glm::ortho(-Window::Get().GetAspectRatio() * Size, Window::Get().GetAspectRatio() * Size, -Size, Size, Near, Far);
		}
	}();
}

const glm::mat4& Camera::GetProjection() const
{
	return m_projection;
}
