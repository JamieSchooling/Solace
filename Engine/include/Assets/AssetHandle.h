#pragma once

#include <stduuid/uuid.h>

#include <filesystem>

using AssetHandle = uuids::uuid;

enum class AssetRelativeRoot
{
	Custom,
	Resources
};

struct AssetMetadata
{
	std::filesystem::path RelativePath;
	AssetRelativeRoot RelativeTo = AssetRelativeRoot::Custom;
};