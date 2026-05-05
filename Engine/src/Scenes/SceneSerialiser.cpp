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

	auto componentReflections = ReflectionRegistry::View(m_Scene.Registry, entity);
	for (auto& component : componentReflections)
	{
		for (auto property : component->GetProperties())
		{
			out[component->Name()].push_back(SerialiseProperty(property, entity));
		}
	}

	return out;
}

JSON SceneSerialiser::SerialiseProperty(IProperty* property, entt::entity entity)
{
	JSON out;
	if (property->Type() == PropertyType::Bool)
	{
		bool value = std::any_cast<bool>(property->Get(m_Scene.Registry, entity));
		out[property->Name()]["Type"] = "bool";
		out[property->Name()]["Value"] = value;
	}
	else if (property->Type() == PropertyType::Int)
	{
		int value = std::any_cast<int>(property->Get(m_Scene.Registry, entity));
		out[property->Name()]["Type"] = "int";
		out[property->Name()]["Value"] = value;
	}
	else if (property->Type() == PropertyType::Float)
	{
		float value = std::any_cast<float>(property->Get(m_Scene.Registry, entity));
		out[property->Name()]["Type"] = "float";
		out[property->Name()]["Value"] = value;
	}
	else if (property->Type() == PropertyType::Vec2)
	{
		glm::vec2 value = std::any_cast<glm::vec2>(property->Get(m_Scene.Registry, entity));
		out[property->Name()]["Type"] = "vec2";
		out[property->Name()]["Value"] = { value.x, value.y };
	}
	else if (property->Type() == PropertyType::Vec3)
	{
		glm::vec3 value = std::any_cast<glm::vec3>(property->Get(m_Scene.Registry, entity));
		out[property->Name()]["Type"] = "vec3";
		out[property->Name()]["Value"] = { value.x, value.y, value.z };
	}
	else if (property->Type() == PropertyType::Vec4)
	{
		glm::vec4 value = std::any_cast<glm::vec4>(property->Get(m_Scene.Registry, entity));
		out[property->Name()]["Type"] = "vec4";
		out[property->Name()]["Value"] = { value.x, value.y, value.z, value.w };
	}
	else if (property->Type() == PropertyType::Quaternion)
	{
		glm::quat value = std::any_cast<glm::quat>(property->Get(m_Scene.Registry, entity));
		out[property->Name()]["Type"] = "quaternion";
		out[property->Name()]["Value"] = { value.x, value.y, value.z, value.w };
	}
	else if (property->Type() == PropertyType::String)
	{
		std::string value = std::any_cast<std::string>(property->Get(m_Scene.Registry, entity));
		out[property->Name()]["Type"] = "string";
		out[property->Name()]["Value"] = value;
	}
	return out;
}

void SceneSerialiser::DeserialiseEntity(JSON entityData)
{
	entt::entity entity = m_Scene.Registry.create();
	m_Scene.Registry.emplace<NameComponent>(entity).Name = entityData["Name"];

	for (auto& [name, compData] : entityData.items())
	{
		auto component = ReflectionRegistry::Get(name.c_str());
		component->Emplace(m_Scene.Registry, entity);
		for (auto& [name, propData] : compData.items())
		{
			auto property = component->GetProperty(name.c_str());
			DeserialiseProperty(propData, property, entity);
		}
	}
	/*if (entityData.contains("Transform"))
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
	}*/
}

void SceneSerialiser::DeserialiseProperty(JSON propertyData, IProperty* property, entt::entity entity)
{
	if (propertyData["Type"] == "bool")
	{
		bool value = propertyData["Value"].get<bool>();
		property->Set(value, m_Scene.Registry, entity);
	}
	else if (propertyData["Type"] == "int")
	{
		int value = propertyData["Value"].get<int>();
		property->Set(value, m_Scene.Registry, entity);
	}
	else if (propertyData["Type"] == "float")
	{
		float value = propertyData["Value"].get<float>();
		property->Set(value, m_Scene.Registry, entity);
	}
	else if (propertyData["Type"] == "vec2")
	{
		auto value = propertyData["Value"];
		glm::vec2 vec = glm::vec2(value[0].get<float>(), value[1].get<float>());
		property->Set(vec, m_Scene.Registry, entity);
	}
	else if (propertyData["Type"] == "vec3")
	{
		auto value = propertyData["Value"];
		glm::vec3 vec = glm::vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
		property->Set(vec, m_Scene.Registry, entity);
	}
	else if (propertyData["Type"] == "vec4")
	{
		auto value = propertyData["Value"];
		glm::vec4 vec = glm::vec4(value[0].get<float>(), value[1].get<float>(), value[2].get<float>(), value[3].get<float>());
		property->Set(vec, m_Scene.Registry, entity);
	}
	else if (propertyData["Type"] == "quaternion")
	{
		auto value = propertyData["Value"];
		glm::quat vec = glm::quat(value[3].get<float>(), value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
		property->Set(vec, m_Scene.Registry, entity);
	}
	else if (propertyData["Type"] == "string")
	{
		std::string value = propertyData["Value"].get<std::string>();
		property->Set(value, m_Scene.Registry, entity);
	}
}