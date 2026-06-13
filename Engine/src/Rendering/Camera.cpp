#include "Rendering/Camera.h"

Camera::Camera()
{
	Initialise();
}

void Camera::Initialise()
{
	RecalculateProjection();
}

glm::mat4 Camera::GetView(Transform& transform)
{
	return glm::lookAt(transform.Position, transform.Position + transform.Forward(), glm::vec3(0.0f, 1.0f, 0.0f));
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

void Camera::RecalculateProjection(glm::vec2 size)
{
	float aspect = size.x / size.y;

	m_projection = [&]()
	{
		switch (ProjectionType)
		{
		case CameraProjectionType::Perspective: return glm::perspective(glm::radians(FOV), aspect, Near, Far);
		case CameraProjectionType::Orthographic: return glm::ortho(-aspect * Size, aspect * Size, -Size, Size, Near, Far);
		}
	}();
}

const glm::mat4& Camera::GetProjection() const
{
	return m_projection;
}
