#include "Scenes/SceneSerialiser.h"

#include "Rendering/Camera.h"
#include "Scenes/NameComponent.h"
#include "Transform/Transform.h"

#include <filesystem>
#include <sstream>
#include <fstream>

SceneSerialiser::SceneSerialiser(Scene& scene) : m_Scene(scene)
{
}

void SceneSerialiser::SerialiseTo(std::filesystem::path path)
{
	JSON out;
	out["Scene"] = m_Scene.Name;
	for (auto& entity : m_Scene.Registry.view<entt::entity>())
	{
		out["Entities"].push_back(SerialiseEntity(entity));
	}

	std::filesystem::path directory = path.parent_path();

	// Check if the directory path is not empty (handles cases where filepath is just a filename)
	if (!directory.empty())
	{
		// Create directories if they don't exist
		if (!std::filesystem::exists(directory))
		{
			if (!std::filesystem::create_directories(directory))
			{
				throw std::runtime_error("Failed to create directories: " + directory.string());
			}
		}
	}

	std::ofstream file(path);
	if (file.is_open())
	{
		file << out.dump(4); // 4 spaces indentation
		file.close();
	}
	else
	{
		// Handle file opening errors appropriately
		throw std::runtime_error("Unable to open file for writing: " + path.string());
	}
}

void SceneSerialiser::DeserialiseFrom(std::filesystem::path path)
{
	m_Scene.Registry.clear();

	std::ifstream stream(path);
	std::stringstream sstream;
	sstream << stream.rdbuf();

	JSON data = JSON::parse(sstream.str());
	if (!data.contains("Scene")) return;
	if (!data.contains("Entities")) return;

	m_Scene.Name = data["Scene"];
	for (auto& entity : data["Entities"])
	{
		DeserialiseEntity(entity);
	}
}

JSON SceneSerialiser::SerialiseEntity(entt::entity entity)
{
	JSON out;

	std::string name = "Unnamed Entity";
	if (m_Scene.Registry.all_of<NameComponent>(entity))
	{
		name = m_Scene.Registry.get<NameComponent>(entity).Name;
	}
	out["Name"] = name;

	if (m_Scene.Registry.all_of<Transform>(entity))
	{
		auto& transform = m_Scene.Registry.get<Transform>(entity);
		out["Transform"]["Position"] = {transform.Position.x, transform.Position.y, transform.Position.z};
		out["Transform"]["Rotation"] = {transform.Rotation.w, transform.Rotation.x, transform.Rotation.y, transform.Rotation.z};
		out["Transform"]["Scale"] = {transform.Scale.x, transform.Scale.y, transform.Scale.z};
	}

	if (m_Scene.Registry.all_of<Camera>(entity))
	{
		auto& camera = m_Scene.Registry.get<Camera>(entity);
		out["Camera"]["FOV"] = camera.FOV;
		out["Camera"]["Near"] = camera.Near;
		out["Camera"]["Far"] = camera.Far;
		out["Camera"]["ProjectionType"] = camera.ProjectionType;
	}

	return out;
}

void SceneSerialiser::DeserialiseEntity(JSON entityData)
{
	entt::entity entity = m_Scene.Registry.create();
	m_Scene.Registry.emplace<NameComponent>(entity).Name = entityData["Name"];

	if (entityData.contains("Transform"))
	{
		auto& transform = m_Scene.Registry.emplace<Transform>(entity);
		auto position = entityData["Transform"]["Position"];
		auto rotation = entityData["Transform"]["Rotation"];
		auto scale = entityData["Transform"]["Scale"];
		transform.Position = glm::vec3(position[0].get<float>(), position[1].get<float>(), position[2].get<float>());
		transform.Rotation = glm::quat(rotation[0].get<float>(), rotation[1].get<float>(), rotation[2].get<float>(), rotation[3].get<float>());
		transform.Scale = glm::vec3(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
	}

	if (entityData.contains("Camera"))
	{
		auto& camera = m_Scene.Registry.emplace<Camera>(entity);
		camera.FOV = entityData["Camera"]["FOV"];
		camera.Near = entityData["Camera"]["Near"];
		camera.Far = entityData["Camera"]["Far"];
		camera.ProjectionType = entityData["Camera"]["ProjectionType"];
		camera.RecalculateProjection();
		m_Scene.MainCamera = entity;
	}
}
