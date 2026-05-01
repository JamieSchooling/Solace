#pragma once

#include <nlohmann/json.hpp>
#include "Scenes/Scene.h"

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
	Scene& m_Scene;

	JSON SerialiseEntity(entt::entity entity);
	void DeserialiseEntity(JSON entityData);
};