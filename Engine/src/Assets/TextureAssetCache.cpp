#include "Assets/TextureAssetCache.h"

#include "Assets/AssetRegistry.h"

std::shared_ptr<Texture> TextureAssetCache::Load(AssetHandle handle)
{
	if (m_cache.contains(handle))
	{
		return m_cache.at(handle);
	}

	if (!AssetRegistry::Get().Exists(handle))
	{
		return nullptr;
	}

	auto path = AssetRegistry::Get().GetFullPath(handle);
	if (path.empty() || !std::filesystem::exists(path))
	{
		return nullptr;
	}

	std::shared_ptr<Texture> texture = std::make_shared<Texture>(path);
	m_cache[handle] = texture;
	return texture;
}