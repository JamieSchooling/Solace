#include "Demo.h"

#include <Core/EntryPoint.h>
#include <Events/EventSystem.h>
#include <Rendering/Window.h>
#include <numeric>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Application* CreateApplication()
{
	return new DemoApp();
}

void DemoApp::Initialise()
{
	AddSubsystem<EventSystem>();

	{
		WindowProps props;
		props.title = "Demo";
		props.eventSystem = &EventSystem::Get();
		AddSubsystem<Window>(props);
	}

	auto renderQueue = std::make_shared<RenderQueue>(); // Could create in a better place

	/*
	* Concept:
	* On update, SceneManager processes current scene, submits to render queue
	*
	* SceneManagerProps props;
	* props.renderQueue = renderQueue;
	*
	* AddSubsystem<SceneManager>(props);
	*/

	// Temp Cube
	std::vector<float> verts = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	std::vector<uint32_t> indices(verts.size() / 3);
	std::iota(indices.begin(), indices.end(), 0);

	//Temp camera data
	glm::mat4 projection = glm::perspective(30.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	VAO geometry(indices);
	geometry.AddVertexBuffer(verts, { {3, ShaderDataType::Float, false, 0} });
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("./resources/shaders/Vertex.glsl", "./resources/shaders/Fragment.glsl");
	Material material(shader);
	glm::mat4 model = glm::translate(glm::mat4(1.0f),
		glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	material.SetValue("u_Model", model);
	material.SetValue("u_MVP", projection * view * model);
	material.SetValue("u_Colour", glm::vec3(1.0));
	RenderItem item{geometry, material};
	renderQueue->push_back(item);

	{
		RenderSystemProps props;
		props.renderQueue = renderQueue;

		AddSubsystem<RenderSystem>(props);
	}
	
	
}

void DemoApp::Run()
{
	while (Window::Get().IsOpen())
	{
		UpdateSubsystems();
	}
}

void DemoApp::Shutdown()
{
	RemoveSubsystem<Window>();
	RemoveSubsystem<EventSystem>();
}
