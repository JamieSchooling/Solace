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

enum class ProjectLoadResult
{
	Unknown,
	Success,
	AlreadyOpen,
	Deleted,
	Package,
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
	std::filesystem::path GetProjectDataPath() { return m_currentProjectDataPath; }
	std::filesystem::path GetProjectLibraryPath() { return m_currentProjectLibraryPath; }
	AssetHandle GetStartupScene() { return m_startupScene; }

	void SerialiseProjectData(AssetHandle startupScene);
	void FreeProject();
private:
	bool m_isProjectLoaded = false;
	ProjectManagerMode m_mode = ProjectManagerMode::List;
	std::string m_currentProjectName = "Untitled";
	std::filesystem::path m_currentProjectPath;
	std::filesystem::path m_currentProjectAssetsPath;
	std::filesystem::path m_currentProjectDataPath;
	std::filesystem::path m_currentProjectLibraryPath;
	AssetHandle m_startupScene;
	bool m_showProjectOpenModal = false;
	bool m_showProjectDeletedModal = false;

	std::filesystem::path m_currentPackageSource;

	std::vector<ProjectInfo> m_projectList;

	void DrawProjectList(ImGuiWindowFlags flags);
	void DrawProjectCreator(ImGuiWindowFlags flags);

	ProjectLoadResult LoadProject(std::filesystem::path projectPath);
	void ShowProjectExistanceWarn();
	void ShowProjectOpenModal();

	void CreateProject(std::filesystem::path projectPath);
	void CreateProjectFromPackage(std::filesystem::path projectPath);
	JSON CreateProjectJson();
	ProjectInfo GetProjectInfo(std::filesystem::path projectPath);
	void SerialiseProjectList();
	bool ExistsInProjectList(std::filesystem::path projectPath);

	bool ProjectEntry(const std::string& projectName, const std::string& projectPath, time_t timestamp, const ImVec2& size);
	

	const char* m_cmakeTemplate = R"CMAKE(
	cmake_minimum_required(VERSION 4.4)
	project ([PROJECT_NAME])

	add_library(GAME SHARED)

	file(GLOB_RECURSE sources CONFIGURE_DEPENDS "[PROJECT_ASSET_PATH]/*.cpp" "[PROJECT_ASSET_PATH]/*.h")
	target_sources(GAME PRIVATE ${sources})

	set_property(TARGET GAME PROPERTY CXX_STANDARD 20)
	)CMAKE";
};