#include "Assets/AssetRegistry.h"
#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
#include <Core/Application.h>

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
	m_metadataByHandle.clear();
}

AssetHandle AssetRegistry::RegisterNewAsset(std::filesystem::path& path, AssetRelativeRoot relativeTo /*=AssetRelativeRoot::Custom*/)
{
	if (std::filesystem::is_directory(path)) { return AssetHandle(); }

	switch (relativeTo)
	{
	case AssetRelativeRoot::Custom:
		path = std::filesystem::relative(path, m_root);
		break;
	case AssetRelativeRoot::Resources:
		path = std::filesystem::relative(path, Application::GetResourcePath());
		break;
	}

	if (m_handleByPath.contains(path)) { return m_handleByPath.at(path); }

	std::random_device rd;
	auto seed_data = std::array<int, std::mt19937::state_size>{};
	std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	std::mt19937 generator(seq);
	uuids::uuid_random_generator gen{ generator };
	
	AssetHandle handle = gen();
	AssetMetadata metadata;
	metadata.RelativePath = path;
	metadata.RelativeTo = relativeTo;
	m_metadataByHandle[handle] = metadata;
	m_handleByPath[path] = handle;

	SerialiseRegistry();
	return handle;
}

void AssetRegistry::MoveAsset(AssetHandle handle, std::filesystem::path newPath)
{
	if (std::filesystem::is_directory(newPath)) { return; }

	if (!m_metadataByHandle.contains(handle)) { return; }
	
	AssetMetadata metadata = m_metadataByHandle.at(handle);
	switch (metadata.RelativeTo)
	{
	case AssetRelativeRoot::Custom:
		newPath = std::filesystem::relative(newPath, m_root);
		break;
	case AssetRelativeRoot::Resources:
		newPath = std::filesystem::relative(newPath, Application::GetResourcePath());
		break;
	}
	std::filesystem::path absolutePath = GetFullPath(handle);
	m_handleByPath.erase(absolutePath);
	m_metadataByHandle[handle].RelativePath = newPath;
	m_handleByPath[newPath] = handle;
	SerialiseRegistry();
}

void AssetRegistry::MoveDirectory(std::filesystem::path oldDir, std::filesystem::path newDir)
{
	std::filesystem::path oldDirRelative = std::filesystem::relative(oldDir, m_root);

	for (auto& [handle, path] : m_metadataByHandle)
	{
		if (path.RelativePath.string().starts_with(oldDirRelative.string()))
		{
			MoveAsset(handle, newDir / path.RelativePath.filename());
		}
	}
}

void AssetRegistry::DeleteAsset(AssetHandle handle)
{
	m_handleByPath.erase(GetFullPath(handle));
	m_metadataByHandle.erase(handle);
	SerialiseRegistry();
}

void AssetRegistry::DeleteDirectory(std::filesystem::path directory)
{
	std::filesystem::path directoryRelative = std::filesystem::relative(directory, m_root);

	std::vector<AssetHandle> toDelete;

	for (const auto& [handle, metadata] : m_metadataByHandle)
	{
		if (metadata.RelativePath.string().starts_with(directoryRelative.string()))
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
	if (!m_metadataByHandle.contains(handle))
	{
		return std::filesystem::path("");
	}
	return m_metadataByHandle.at(handle).RelativePath;
}

std::filesystem::path AssetRegistry::GetFullPath(AssetHandle handle) const
{
	if (!m_metadataByHandle.contains(handle) || m_metadataByHandle.at(handle).RelativePath.empty())
	{
		return std::filesystem::path("");
	}
	AssetMetadata metadata = m_metadataByHandle.at(handle);
	switch (metadata.RelativeTo)
	{
	case AssetRelativeRoot::Custom:
		return m_root / metadata.RelativePath;
	case AssetRelativeRoot::Resources:
		return Application::GetResourcePath() / metadata.RelativePath;
	}
}

void AssetRegistry::SerialiseRegistry()
{
	JSON json;
	for (auto& [handle, metadata] : m_metadataByHandle)
	{
		JSON meta;
		meta["Path"] = metadata.RelativePath;
		switch (metadata.RelativeTo)
		{
		case AssetRelativeRoot::Custom:
			meta["RelativeTo"] = "Custom";
			break;
		case AssetRelativeRoot::Resources:
			meta["RelativeTo"] = "Resources";
			break;
		}
		json[uuids::to_string(handle)] = meta;
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

		for (auto& [handle, meta] : data.items())
		{
			auto h = AssetHandle::from_string(handle);
			if (!h.has_value()) { continue; }
			AssetHandle handle = h.value();
			AssetMetadata metadata;
			std::filesystem::path relativePath = meta["Path"].get<std::string>();
			metadata.RelativePath = relativePath;
			std::filesystem::path absolutePath;
			if (meta["RelativeTo"] == "Custom")
			{
				metadata.RelativeTo = AssetRelativeRoot::Custom;
				absolutePath = m_root / relativePath;
			}
			else if (meta["RelativeTo"] == "Resources")
			{
				metadata.RelativeTo = AssetRelativeRoot::Resources;
				absolutePath = Application::GetResourcePath() / relativePath;
			}
			m_metadataByHandle[handle] = metadata;
			m_handleByPath[relativePath] = handle;
		}
	}
}
