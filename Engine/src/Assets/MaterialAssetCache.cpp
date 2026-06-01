#include "Assets/MaterialAssetCache.h"

#include "Assets/MaterialSerialiser.h"
#include "Assets/AssetRegistry.h"

std::shared_ptr<Material> MaterialAssetCache::Load(AssetHandle handle)
{
	if (m_cache.contains(handle))
	{
		return m_cache.at(handle);
	}

	std::shared_ptr<Material> material;
	if (!AssetRegistry::Get().Exists(handle))
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>("./resources/shaders/Vertex.glsl", "./resources/shaders/Fragment.glsl"); 
		material = std::make_shared<::Material>(shader, InitWithDefaultValues);
		material->SetValue("u_prop_colour", glm::vec3(1.0));
	}
	else
	{
		auto path = AssetRegistry::Get().GetFullPath(handle);
		MaterialSerialiser ms;
		material = ms.DeserialiseFrom(path);
	}

	m_cache[handle] = material;
	return material;
}
