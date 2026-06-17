#pragma once

#include <filesystem>

#include <imgui.h>

#include <nlohmann/json.hpp>

#include <Assets/AssetHandle.h>

using JSON = nlohmann::ordered_json;

enum class ProjectManagerMode
{
	List,
	Create,
	CreateFromPackage,
};

struct ProjectInfo
{
	std::string Name;
	std::filesystem::path Path;
	std::filesystem::path AssetsDirectoryPath;
	time_t LastModifiedTimestamp;
	AssetHandle StartupScene;
};

class ProjectManager
{
public:
	void Initialise(std::filesystem::path projectPath);
	void Update();

	static bool IsProjectFile(std::filesystem::path);
	bool IsProjectLoaded();
	std::filesystem::path GetProjectPath() { return m_currentProjectPath; }
	std::filesystem::path GetProjectAssetsPath() { return m_currentProjectAssetsPath; }
	AssetHandle GetStartupScene() { return m_startupScene; }

	void SerialiseProjectData(AssetHandle startupScene);
private:
	bool m_isProjectLoaded = false;
	ProjectManagerMode m_mode = ProjectManagerMode::List;
	std::string m_currentProjectName = "Untitled";
	std::filesystem::path m_currentProjectPath;
	std::filesystem::path m_currentProjectAssetsPath;
	AssetHandle m_startupScene;

	std::filesystem::path m_currentPackageSource;

	std::vector<ProjectInfo> m_projectList;

	void DrawProjectList(ImGuiWindowFlags flags);
	void DrawProjectCreator(ImGuiWindowFlags flags);

	bool LoadProject(std::filesystem::path projectPath);
	bool ShowProjectExistanceWarn(bool show);

	void CreateProject(std::filesystem::path projectPath);
	void CreateProjectFromPackage(std::filesystem::path projectPath);
	JSON CreateProjectJson();
	ProjectInfo GetProjectInfo(std::filesystem::path projectPath);
	void SerialiseProjectList();
	bool ExistsInProjectList(std::filesystem::path projectPath);

	bool ProjectEntry(const std::string& projectName, const std::string& projectPath, time_t timestamp, const ImVec2& size);
	
};