#pragma once

#include "Rendering/Material.h"
#include <filesystem>

class MaterialAssetCache
{
public:
	static std::shared_ptr<Material> Load(std::filesystem::path path);
private:
	static inline std::unordered_map< std::filesystem::path, std::shared_ptr<Material>> m_cache;
};