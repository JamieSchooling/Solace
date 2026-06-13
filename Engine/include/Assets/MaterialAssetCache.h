#pragma once

#include "Rendering/Material.h"
#include <filesystem>

#include "Assets/AssetHandle.h"

class MaterialAssetCache
{
public:
	static std::shared_ptr<Material> Load(AssetHandle handle);
	static void Delete(AssetHandle handle);
private:
	static inline std::unordered_map<AssetHandle, std::shared_ptr<Material>> m_cache;
};