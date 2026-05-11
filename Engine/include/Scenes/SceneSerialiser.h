#pragma once

#include <nlohmann/json.hpp>
#include "Scenes/Scene.h"
#include <Reflection/Property.h>

using JSON = nlohmann::ordered_json;
// Forward declarations
namespace std { namespace filesystem { class path; } }

class SceneSerialiser
{
public:
	SceneSerialiser(Scene& scene);

	void SerialiseTo(std::filesystem::path path);
	void DeserialiseFrom(std::filesystem::path path);

private:
	Scene& m_scene;

	JSON SerialiseEntity(entt::entity entity);
	JSON SerialiseProperty(IProperty* property, entt::entity entity);
	void DeserialiseEntity(JSON entityData);
	void DeserialiseProperty(JSON propertyData, IProperty* property, entt::entity entity);
};