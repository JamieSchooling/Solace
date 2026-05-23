#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

#include "Rendering/Material.h"

using JSON = nlohmann::ordered_json;

class Material;

class MaterialSerialiser
{
public:	
	void SerialiseTo(std::shared_ptr<Material> material, std::filesystem::path path);
	std::shared_ptr<Material> DeserialiseFrom(std::filesystem::path path);

private:
	JSON SerialiseProperty(const std::string& name, UniformData data, UniformDescription desc);
	void DeserialiseProperty(JSON propertyData, std::shared_ptr<Material> material);
};