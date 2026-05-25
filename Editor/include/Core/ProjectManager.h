#pragma once

#include <filesystem>

#include <imgui.h>

#include <nlohmann/json.hpp>

using JSON = nlohmann::ordered_json;

enum class ProjectManagerMode
{
	List,
	Create
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
private:
	bool m_isProjectLoaded = false;
	ProjectManagerMode m_mode = ProjectManagerMode::List;
	std::string m_currentProjectName = "Untitled";
	std::filesystem::path m_currentProjectPath;
	std::filesystem::path m_currentProjectAssetsPath;

	std::vector<std::filesystem::path> m_projectList;

	void DrawProjectList(ImGuiWindowFlags flags);
	void DrawProjectCreator(ImGuiWindowFlags flags);

	void LoadProject(std::filesystem::path projectPath);	
	void CreateProject(std::filesystem::path projectPath);
	JSON CreateProjectJson();
	void SerialiseProjectList();
	bool ExistsInProjectList(std::filesystem::path projectPath);

	bool ProjectEntry(const std::string& projectName, const std::string& projectPath, const ImVec2& size);
	
};