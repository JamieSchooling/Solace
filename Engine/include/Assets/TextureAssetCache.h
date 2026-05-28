#pragma once

#include "Rendering/Texture.h"
#include <filesystem>
#include "AssetHandle.h"

class TextureAssetCache
{
public:
	// Will return nullptr if image asset doesn't exist / is not found
	static std::shared_ptr<Texture> Load(AssetHandle handle);
private:
	static inline std::unordered_map<AssetHandle, std::shared_ptr<Texture>> m_cache;
};