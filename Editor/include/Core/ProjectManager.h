#pragma once

#include <filesystem>

#include <imgui.h>

class ProjectManager
{
public:
	void Initialise(std::filesystem::path projectPath);
	void Update();

	bool IsProjectLoaded();
private:
	bool m_isProjectLoaded = false;

	void LoadProject(std::filesystem::path projectPath);	
	void CreateProject(std::filesystem::path projectDirectory);

	bool ProjectEntry(const std::string& projectName, const std::string& projectPath, const ImVec2& size);
	
};