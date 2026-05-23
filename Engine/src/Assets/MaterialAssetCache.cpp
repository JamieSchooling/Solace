#include "Assets/MaterialAssetCache.h"
#include "Assets/MaterialSerialiser.h"

std::shared_ptr<Material> MaterialAssetCache::Load(std::filesystem::path path)
{
	if (m_cache.contains(path))
	{
		return m_cache.at(path);
	}

	MaterialSerialiser ms;
	auto material = ms.DeserialiseFrom(path);
	m_cache[path] = material;
	return material;
}
