#pragma once

#include "Core/Subsystem.h"

#include <filesystem>
#include <unordered_map>

#include "Assets/AssetHandle.h"

struct AssetRegistryProps : public SubsystemParams
{
	std::filesystem::path Root;
	std::filesystem::path RegistryFile;
};

class AssetRegistry : public SingletonSubsystem<AssetRegistry>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	AssetHandle RegisterNewAsset(std::filesystem::path path);
	void MoveAsset(AssetHandle handle, std::filesystem::path newPath);
	void MoveDirectory(std::filesystem::path oldDir, std::filesystem::path newDir);

	void DeleteAsset(AssetHandle handle);
	void DeleteDirectory(std::filesystem::path directory);

	AssetHandle GetHandle(const std::filesystem::path& path);
	std::filesystem::path GetPath(AssetHandle handle) const;
	std::filesystem::path GetFullPath(AssetHandle handle) const;

private:
	std::unordered_map<AssetHandle, std::filesystem::path> m_pathByHandle;
	std::unordered_map<std::filesystem::path, AssetHandle> m_handleByPath;

	std::filesystem::path m_root;
	std::filesystem::path m_registryFile;

	void SerialiseRegistry();
	void DeserialiseRegistry();
};