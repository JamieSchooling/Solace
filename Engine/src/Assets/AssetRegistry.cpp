#include "Assets/AssetRegistry.h"
#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

using JSON = nlohmann::ordered_json;

void AssetRegistry::Start(const SubsystemParams& params)
{
	const AssetRegistryProps& props = static_cast<const AssetRegistryProps&>(params);

	m_root = props.Root;
	m_registryFile = props.RegistryFile;

	DeserialiseRegistry();
}

void AssetRegistry::Shutdown()
{
	m_handleByPath.clear();
	m_pathByHandle.clear();
}

AssetHandle AssetRegistry::RegisterNewAsset(std::filesystem::path path)
{
	if (m_handleByPath.contains(path)) { return m_handleByPath.at(path); }

	std::random_device rd;
	auto seed_data = std::array<int, std::mt19937::state_size>{};
	std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	std::mt19937 generator(seq);
	uuids::uuid_random_generator gen{ generator };

	path = std::filesystem::relative(path, m_root);
	AssetHandle handle = gen();
	m_pathByHandle[handle] = path;
	m_handleByPath[path] = handle;
	SerialiseRegistry();
	return handle;
}

void AssetRegistry::MoveAsset(AssetHandle handle, std::filesystem::path newPath)
{
	if (!m_pathByHandle.contains(handle)) { return; }

	newPath = std::filesystem::relative(newPath, m_root);
	m_handleByPath.erase(m_pathByHandle.at(handle));
	m_pathByHandle[handle] = newPath;
	m_handleByPath[newPath] = handle;
	SerialiseRegistry();
}

void AssetRegistry::MoveDirectory(std::filesystem::path oldDir, std::filesystem::path newDir)
{
	std::filesystem::path oldDirRelative = std::filesystem::relative(oldDir, m_root);

	for (auto& [handle, path] : m_pathByHandle)
	{
		if (path.string().starts_with(oldDirRelative.string()))
		{
			MoveAsset(handle, newDir / path.filename());
		}
	}
}

void AssetRegistry::DeleteAsset(AssetHandle handle)
{
	m_handleByPath.erase(m_pathByHandle.at(handle));
	m_pathByHandle.erase(handle);
	SerialiseRegistry();
}

void AssetRegistry::DeleteDirectory(std::filesystem::path directory)
{
	std::filesystem::path directoryRelative = std::filesystem::relative(directory, m_root);

	std::vector<AssetHandle> toDelete;

	for (const auto& [handle, path] : m_pathByHandle)
	{
		if (path.string().starts_with(directoryRelative.string()))
		{
			toDelete.push_back(handle);
		}
	}

	for (AssetHandle handle : toDelete)
	{
		DeleteAsset(handle);
	}
}

AssetHandle AssetRegistry::GetHandle(const std::filesystem::path& path)
{
	if (!m_handleByPath.contains(path))
	{
		return AssetHandle();
	}

	return m_handleByPath.at(path);
}

std::filesystem::path AssetRegistry::GetPath(AssetHandle handle) const
{
	if (!m_pathByHandle.contains(handle))
	{
		return std::filesystem::path("");
	}
	return m_pathByHandle.at(handle);
}

std::filesystem::path AssetRegistry::GetFullPath(AssetHandle handle) const
{
	if (!m_pathByHandle.contains(handle))
	{
		return std::filesystem::path("");
	}
	return m_root / GetPath(handle);
}

void AssetRegistry::SerialiseRegistry()
{
	JSON json;
	for (auto& [handle, path] : m_pathByHandle)
	{
		json[uuids::to_string(handle)] = path;
	}
	std::filesystem::create_directories(m_registryFile.parent_path());
	std::ofstream file(m_registryFile);
	if (file.is_open())
	{
		file << json.dump(4); // 4 spaces indentation
		file.close();
	}
	else
	{
		// Handle file opening errors appropriately
		throw std::runtime_error("Unable to open file for writing: " + m_registryFile.string());
	}
}

void AssetRegistry::DeserialiseRegistry()
{
	if (std::filesystem::exists(m_registryFile))
	{
		std::ifstream stream(m_registryFile);
		std::stringstream sstream;
		sstream << stream.rdbuf();

		JSON data = JSON::parse(sstream.str());

		for (auto& [handle, path] : data.items())
		{
			auto h = AssetHandle::from_string(handle);
			if (!h.has_value()) { continue; }
			AssetHandle handle = h.value();
			m_pathByHandle[handle] = path.get<std::string>();
			m_handleByPath[path] = handle;
		}
	}
}
