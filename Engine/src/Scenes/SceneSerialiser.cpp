#include "Scenes/SceneSerialiser.h"

#include "Rendering/Camera.h"
#include "Scenes/NameComponent.h"
#include "Transform/Transform.h"

#include <filesystem>
#include <sstream>
#include <fstream>

SceneSerialiser::SceneSerialiser(Scene& scene) : m_scene(scene)
{
}

void SceneSerialiser::SerialiseTo(std::filesystem::path path)
{
	JSON out;
	out["Scene"] = m_scene.Name;
	for (auto& entity : m_scene.Registry.view<entt::entity>())
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
	m_scene.Registry.clear();

	std::ifstream stream(path);
	std::stringstream sstream;
	sstream << stream.rdbuf();

	JSON data = JSON::parse(sstream.str());
	if (!data.contains("Scene")) return;
	if (!data.contains("Entities")) return;

	m_scene.Name = data["Scene"];
	for (auto& entity : data["Entities"])
	{
		DeserialiseEntity(entity);
	}
}

JSON SceneSerialiser::SerialiseEntity(entt::entity entity)
{
	JSON out;
	std::string m_name = "Unnamed Entity";
	if (m_scene.Registry.all_of<NameComponent>(entity))
	{
		m_name = m_scene.Registry.get<NameComponent>(entity).Name;
	}
	out["Name"] = m_name;

	auto componentReflections = ReflectionRegistry::View(m_scene.Registry, entity);
	for (auto& component : componentReflections)
	{
		for (auto property : component->GetProperties())
		{
			out[component->Name()][property->Name()] = SerialiseProperty(property, entity);
		}
	}

	return out;
}

JSON SceneSerialiser::SerialiseProperty(IProperty* property, entt::entity entity)
{
	JSON out;
	if (property->Type() == PropertyType::Bool)
	{
		bool value = std::any_cast<bool>(property->Get(m_scene.Registry, entity));
		out["Type"] = "bool";
		out["Value"] = value;
	}
	else if (property->Type() == PropertyType::Int)
	{
		int value = std::any_cast<int>(property->Get(m_scene.Registry, entity));
		out["Type"] = "int";
		out["Value"] = value;
	}
	else if (property->Type() == PropertyType::Float)
	{
		float value = std::any_cast<float>(property->Get(m_scene.Registry, entity));
		out["Type"] = "float";
		out["Value"] = value;
	}
	else if (property->Type() == PropertyType::Vec2)
	{
		glm::vec2 value = std::any_cast<glm::vec2>(property->Get(m_scene.Registry, entity));
		out["Type"] = "vec2";
		out["Value"] = { value.x, value.y };
	}
	else if (property->Type() == PropertyType::Vec3)
	{
		glm::vec3 value = std::any_cast<glm::vec3>(property->Get(m_scene.Registry, entity));
		out["Type"] = "vec3";
		out["Value"] = { value.x, value.y, value.z };
	}
	else if (property->Type() == PropertyType::Vec4)
	{
		glm::vec4 value = std::any_cast<glm::vec4>(property->Get(m_scene.Registry, entity));
		out["Type"] = "vec4";
		out["Value"] = { value.x, value.y, value.z, value.w };
	}
	else if (property->Type() == PropertyType::Quaternion)
	{
		glm::quat value = std::any_cast<glm::quat>(property->Get(m_scene.Registry, entity));
		out["Type"] = "quaternion";
		out["Value"] = { value.x, value.y, value.z, value.w };
	}
	else if (property->Type() == PropertyType::String)
	{
		std::string value = std::any_cast<std::string>(property->Get(m_scene.Registry, entity));
		out["Type"] = "string";
		out["Value"] = value;
	}
	return out;
}

void SceneSerialiser::DeserialiseEntity(JSON entityData)
{
	entt::entity entity = m_scene.Registry.create();
	m_scene.Registry.emplace<NameComponent>(entity).Name = entityData["Name"];

	for (auto& [m_name, compData] : entityData.items())
	{
		if (m_name == "Name") continue;
		auto component = ReflectionRegistry::Get(m_name.c_str());
		component->Emplace(m_scene.Registry, entity);
		if (m_name == "Camera")
		{
			m_scene.MainCamera = entity;
		}
		for (auto& [key, propData] : compData.items())
		{
			auto property = component->GetProperty(key.c_str());
			DeserialiseProperty(propData, property, entity);
		}
		component->Initialise(m_scene.Registry, entity);
	}
}

void SceneSerialiser::DeserialiseProperty(JSON propertyData, IProperty* property, entt::entity entity)
{
	if (propertyData["Type"] == "bool")
	{
		bool value = propertyData["Value"].get<bool>();
		property->Set(value, m_scene.Registry, entity);
	}
	else if (propertyData["Type"] == "int")
	{
		int value = propertyData["Value"].get<int>();
		property->Set(value, m_scene.Registry, entity);
	}
	else if (propertyData["Type"] == "float")
	{
		float value = propertyData["Value"].get<float>();
		property->Set(value, m_scene.Registry, entity);
	}
	else if (propertyData["Type"] == "vec2")
	{
		auto value = propertyData["Value"];
		glm::vec2 vec = glm::vec2(value[0].get<float>(), value[1].get<float>());
		property->Set(vec, m_scene.Registry, entity);
	}
	else if (propertyData["Type"] == "vec3")
	{
		auto value = propertyData["Value"];
		glm::vec3 vec = glm::vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
		property->Set(vec, m_scene.Registry, entity);
	}
	else if (propertyData["Type"] == "vec4")
	{
		auto value = propertyData["Value"];
		glm::vec4 vec = glm::vec4(value[0].get<float>(), value[1].get<float>(), value[2].get<float>(), value[3].get<float>());
		property->Set(vec, m_scene.Registry, entity);
	}
	else if (propertyData["Type"] == "quaternion")
	{
		auto value = propertyData["Value"];
		glm::quat quat = glm::quat(value[3].get<float>(), value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
		property->Set(quat, m_scene.Registry, entity);
	}
	else if (propertyData["Type"] == "string")
	{
		std::string value = propertyData["Value"].get<std::string>();
		property->Set(value, m_scene.Registry, entity);
	}
}