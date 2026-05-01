#pragma once

#include "Scenes/Scene.h"
#include <numeric>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Rendering/MeshRender.h>
#include <Transform/Transform.h>
#include "Scenes/NameComponent.h"

Scene BuildDefaultScene()
{
	Scene scene;

	entt::entity cube = scene.Registry.create();
	// Temp Cube
	std::vector<float> verts = {
		// positions			// colours
		-1.0f,  1.0f, -1.0f,	0.1f, 0.1f, 0.1f,
		-1.0f, -1.0f, -1.0f,	0.1f, 0.1f, 0.1f,
		 1.0f, -1.0f, -1.0f,	0.1f, 0.1f, 0.1f,
		 1.0f, -1.0f, -1.0f,	0.1f, 0.1f, 0.1f,
		 1.0f,  1.0f, -1.0f,	0.1f, 0.1f, 0.1f,
		-1.0f,  1.0f, -1.0f,	0.1f, 0.1f, 0.1f,

		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,

		 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,	0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,

		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,

		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,

		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,
	};
	std::vector<uint32_t> indices;
	indices.reserve(verts.size() / 3);

	for (uint32_t i = 0; i < verts.size() / 3; i += 3)
	{
		indices.push_back(i + 0);
		indices.push_back(i + 2);
		indices.push_back(i + 1);
	}

	scene.Registry.emplace<NameComponent>(cube).Name = "Cube";

	std::shared_ptr<VAO> geometry = std::make_shared<VAO>(indices);
	geometry->AddVertexBuffer(verts, { {3, ShaderDataType::Float, false, 0}, {3, ShaderDataType::Float, false, 3 * sizeof(float)}});

	std::shared_ptr<Shader> shader = std::make_shared<Shader>("./resources/shaders/Vertex.glsl", "./resources/shaders/Fragment.glsl");
	std::shared_ptr<Material> material = std::make_shared<Material>(shader);
	material->SetValue("u_Colour", glm::vec3(1.0));

	scene.Registry.emplace<MeshRenderComponent>(cube, material, geometry);
	Transform& transform = scene.Registry.emplace<Transform>(cube);
	transform.Position = glm::vec3(0.0f, 0.0f, -3.0f);
	transform.Rotation = glm::quat(glm::radians(glm::vec3(0.0f, 45.0f, 0.0f)));

	return scene;
}