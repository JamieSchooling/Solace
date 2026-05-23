#include "Assets/MaterialSerialiser.h"

#include <sstream>
#include <fstream>

void MaterialSerialiser::SerialiseTo(std::shared_ptr<Material> material, std::filesystem::path path)
{
	auto [vertexPath, fragmentPath] = material->GetShaderPaths();

	JSON out;
	out["VertexPath"] = vertexPath;
	out["FragmentPath"] = fragmentPath;
	for (auto [name, data] : material->GetUniformData())
	{
		bool isProperty = name.rfind("u_prop_", 0) == 0;
		if (!isProperty) { continue; }

		if (std::holds_alternative<std::monostate>(data))
		{
			std::cout << "Warning: Couldn't serialise [" << name << "], material value not set." << std::endl;
			continue;
		}

		UniformDescription desc = material->GetUniformDescription(name);
		out["Properties"].push_back(SerialiseProperty(name, data, desc));
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

std::shared_ptr<Material> MaterialSerialiser::DeserialiseFrom(std::filesystem::path path)
{
	std::ifstream stream(path);
	std::stringstream sstream;
	sstream << stream.rdbuf();

	JSON data = JSON::parse(sstream.str());

	auto shader = std::make_shared<Shader>(data["VertexPath"].get<std::string>().c_str(), data["FragmentPath"].get<std::string>().c_str());
	auto material = std::make_shared<Material>(shader);
	for (auto propJson : data["Properties"])
	{
		DeserialiseProperty(propJson, material);
	}
	return material;
}

JSON MaterialSerialiser::SerialiseProperty(const std::string& name, UniformData data, UniformDescription desc)
{
	JSON out;

	out["Name"] = name;
	if (desc.type == ShaderDataType::Bool)
	{
		bool value = std::get<bool>(data);
		out["Type"] = "bool";
		out["Value"] = value;
	}
	else if (desc.type == ShaderDataType::Int)
	{
		int value = std::get<int>(data);
		out["Type"] = "int";
		out["Value"] = value;
	}
	else if (desc.type == ShaderDataType::Float)
	{
		float value = std::get<float>(data);
		out["Type"] = "float";
		out["Value"] = value;
	}
	else if (desc.type == ShaderDataType::Vector2)
	{
		glm::vec2 value = std::get<glm::vec2>(data);
		out["Type"] = "vec2";
		out["Value"] = { value.x, value.y };
	}
	else if (desc.type == ShaderDataType::Vector3)
	{
		glm::vec3 value = std::get<glm::vec3>(data);
		out["Type"] = "vec3";
		out["Value"] = { value.x, value.y, value.z };
	}
	else if (desc.type == ShaderDataType::Vector4)
	{
		glm::vec4 value = std::get<glm::vec4>(data);
		out["Type"] = "vec4";
		out["Value"] = { value.x, value.y, value.z, value.w };
	}
	return out;
}

void MaterialSerialiser::DeserialiseProperty(JSON propertyData, std::shared_ptr<Material> material)
{
	std::string name = propertyData["Name"];
	if (propertyData["Type"] == "bool")
	{
		bool value = propertyData["Value"].get<bool>();
		material->SetValue(name, value);
	}
	else if (propertyData["Type"] == "int")
	{
		int value = propertyData["Value"].get<int>();
		material->SetValue(name, value);
	}
	else if (propertyData["Type"] == "float")
	{
		float value = propertyData["Value"].get<float>();
		material->SetValue(name, value);
	}
	else if (propertyData["Type"] == "vec2")
	{
		auto value = propertyData["Value"];
		glm::vec2 vec = glm::vec2(value[0].get<float>(), value[1].get<float>());
		material->SetValue(name, vec);
	}
	else if (propertyData["Type"] == "vec3")
	{
		auto value = propertyData["Value"];
		glm::vec3 vec = glm::vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
		material->SetValue(name, vec);
	}
	else if (propertyData["Type"] == "vec4")
	{
		auto value = propertyData["Value"];
		glm::vec4 vec = glm::vec4(value[0].get<float>(), value[1].get<float>(), value[2].get<float>(), value[3].get<float>());
		material->SetValue(name, vec);
	}
}