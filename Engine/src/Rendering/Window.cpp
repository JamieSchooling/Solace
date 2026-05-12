#include "Rendering/Window.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Events/EventSystem.h"

void Window::Start(const SubsystemParams& params)
{
	const WindowProps& props = static_cast<const WindowProps&>(params);

    m_eventSystem = props.EventSystem;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Request a depth buffer with at least 24 bits
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	m_glfwInstance = glfwCreateWindow(props.Width, props.Height, props.Title, NULL, NULL);
	glfwSetWindowUserPointer(m_glfwInstance, static_cast<void*>(this));

	m_title = props.Title;
    m_width = props.Width;
    m_height = props.Height;
    m_aspectRatio = m_width / m_height;

    if (m_glfwInstance == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_glfwInstance);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, props.Width, props.Height);
    glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

    glfwSetWindowCloseCallback(m_glfwInstance, [](GLFWwindow* window) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        Event e;
        e.Type = EventType::WindowClose;
        self->m_eventSystem->DispatchEvent(e);
    });

    glfwSetFramebufferSizeCallback(m_glfwInstance, [](GLFWwindow* window, int width, int height) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        glViewport(0, 0, width, height);
        self->m_width = width;
        self->m_height = height;
        self->m_aspectRatio = self->m_width / self->m_height;

        Event e;
        e.Type = EventType::WindowResize;
        e.WindowResizeArgs.Width = width;
        e.WindowResizeArgs.Height = height;        
        self->m_eventSystem->DispatchEvent(e);
    });

    glfwSetWindowPosCallback(m_glfwInstance, [](GLFWwindow* window, int x, int y) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        Event e;
        e.Type = EventType::WindowMove;
        e.WindowMoveArgs.PosX = x;
        e.WindowMoveArgs.PosY = y;
        self->m_eventSystem->DispatchEvent(e);
    });

    glfwSetWindowFocusCallback(m_glfwInstance, [](GLFWwindow* window, int focused) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        Event e;
        e.Type = focused == GLFW_TRUE ? EventType::WindowFocus : EventType::WindowFocusLost;
        self->m_eventSystem->DispatchEvent(e);
    });
}

void Window::Shutdown()
{
    glfwTerminate();
}

void Window::PreAppUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::PostAppUpdate()
{
    glfwSwapBuffers(m_glfwInstance);
    glfwPollEvents();
}

bool Window::IsOpen()
{
    return !glfwWindowShouldClose(m_glfwInstance);
}
