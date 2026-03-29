#include "Rendering/Window.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

void Window::Start(SubsystemParams params)
{
	WindowProps props = static_cast<WindowProps>(params);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Request a depth buffer with at least 24 bits
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	m_GLFWInstance = glfwCreateWindow(props.width, props.height, props.title, NULL, NULL);
	glfwSetWindowUserPointer(m_GLFWInstance, static_cast<void*>(this));

	m_Title = props.title;
    if (m_GLFWInstance == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_GLFWInstance);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, props.width, props.height);
    glfwSetFramebufferSizeCallback(m_GLFWInstance, [](GLFWwindow* window, int width, int height) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        glViewport(0, 0, width, height);
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
    glfwSwapBuffers(m_GLFWInstance);
    glfwPollEvents();
}

bool Window::IsOpen()
{
    return !glfwWindowShouldClose(m_GLFWInstance);
}
