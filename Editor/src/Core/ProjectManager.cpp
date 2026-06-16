#include "Core/ProjectManager.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <misc/cpp/imgui_stdlib.h>

#include <Rendering/Window.h>
#include <Core/Application.h>
#include <nfd.h>
#include <nfd_glfw3.h>
#include <iostream>
#include <fstream>
#include <ZipLib/ZipArchive.h>
#include <ZipLib/ZipFile.h>

void ProjectManager::Initialise(std::filesystem::path projectPath)
{
	if (!projectPath.empty())
	{
		LoadProject(projectPath);
	}

	if (std::filesystem::exists(Application::GetDataPath() / "ProjectList.json"))
	{
		std::ifstream stream(Application::GetDataPath() / "ProjectList.json");
		std::stringstream sstream;
		sstream << stream.rdbuf();

		JSON data = JSON::parse(sstream.str());

		for (auto& project : data["Projects"])
		{
			if (std::filesystem::exists(project))
			{
				m_projectList.push_back(project);
			}
		}

		SerialiseProjectList(); // Removes deleted projects from list file
	}
}

void ProjectManager::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoCollapse;

	switch (m_mode)
	{
	case ProjectManagerMode::List:
		DrawProjectList(flags);
		break;
	case ProjectManagerMode::Create:
	case ProjectManagerMode::CreateFromPackage:
		DrawProjectCreator(flags);
		break;
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ProjectManager::IsProjectFile(std::filesystem::path file)
{
	return file.extension() == ".solaceproj" || file.extension() == ".solacepkg";
}

bool ProjectManager::IsProjectLoaded()
{
	return m_isProjectLoaded;
}

void ProjectManager::SerialiseProjectData(AssetHandle startupScene)
{
	if (m_currentProjectPath.empty()) { return; }

	JSON json;
	json["AssetsFolder"] = "Assets";
	json["StartupScene"] = uuids::to_string(startupScene);
	
	std::filesystem::path projectFile = (m_currentProjectPath / m_currentProjectName).replace_extension(".solaceproj");
	std::ofstream file((m_currentProjectPath / m_currentProjectName).replace_extension(".solaceproj").string());
	if (file.is_open())
	{
		file << json.dump(4); // 4 spaces indentation
		file.close();
	}
	else
	{
		// Handle file opening errors appropriately
		throw std::runtime_error("Unable to open file for writing: " + m_currentProjectPath.string());
	}
}

void ProjectManager::DrawProjectList(ImGuiWindowFlags flags)
{
	ImGui::Begin("Project Manager", nullptr, flags);
	ImGui::PushFont(nullptr, 32.0f);
	ImGui::SetCursorPosX(40);
	ImGui::TextUnformatted("Projects");
	ImGui::PopFont();


	ImGui::BeginChild("Project List", ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 44.0f));
	if (m_projectList.empty())
	{
		ImGui::PushFont(nullptr, 20.0f);
		ImGui::SetCursorPosX(35);
		ImGui::TextUnformatted("No projects yet.");
		ImGui::PopFont();
	}
	else
	{
		for (auto& project : m_projectList)
		{
			ImGui::Separator();
			static bool showLoadFailed = false;
			if (ProjectEntry(project.stem().string(), project.string(), ImVec2(ImGui::GetContentRegionAvail().x, 55)))
			{
				showLoadFailed = !LoadProject(project);
			}
			ImGui::PushID(project.string().c_str());
			showLoadFailed = ShowProjectExistanceWarn(showLoadFailed);
			ImGui::PopID();
		}
		ImGui::Separator();
	}
	ImGui::EndChild();

	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 200.0f);
	if (ImGui::Button("Add Project"))
	{
		nfdchar_t* outPath = nullptr;
		nfdopendialogu8args_t args = { 0 };
		nfdu8filteritem_t filters[1] = { { "Solace Project", "solaceproj,solacepkg" } };
		args.filterList = filters;
		args.filterCount = 1;
		NFD_GetNativeWindowFromGLFWWindow(Window::Get().GetGLFWInstance(), &args.parentWindow);
		nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
		if (result == NFD_OKAY)
		{
			LoadProject(outPath);
			NFD_FreePath(outPath);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("+ New Project"))
	{
		m_mode = ProjectManagerMode::Create;
	}

	ImGui::End();
}

void ProjectManager::DrawProjectCreator(ImGuiWindowFlags flags)
{
	ImGui::Begin("Project Creator", nullptr, flags);
	ImGui::PushFont(nullptr, 32.0f);
	ImGui::SetCursorPosX(40);
	ImGui::TextUnformatted(m_mode == ProjectManagerMode::CreateFromPackage ? "Unpackage Project" : "New Project");
	ImGui::PopFont();

	ImGui::SetCursorPosX(40);
	ImGui::TextUnformatted("Project Name");
	ImGui::SetCursorPosX(40);
	if (ImGui::InputText("##Project Name", &m_currentProjectName));

	bool projectAlreadyExists = !m_currentProjectName.empty() && std::filesystem::exists(m_currentProjectPath / m_currentProjectName);
	if (projectAlreadyExists)
	{
		ImGui::SetCursorPosX(40);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8, 0, 0, 1));
		ImGui::TextUnformatted("A project with this name already exists here.");
		ImGui::PopStyleColor();
	}

	
	std::string path = (m_currentProjectPath / m_currentProjectName).string();
	ImGui::SetCursorPosX(40);
	ImGui::TextUnformatted("Project Location");
	ImGui::SetCursorPosX(40);
	ImGui::PushStyleColor(ImGuiCol_InputTextCursor, ImVec4(0, 0, 0, 0));
	ImGui::InputText("##Project Path", &path, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_ElideLeft);
	ImGui::PopStyleColor();
	if (ImGui::IsItemClicked())
	{
		nfdchar_t* outPath = nullptr;
		nfdpickfolderu8args_t args = { 0 };

		NFD_GetNativeWindowFromGLFWWindow(Window::Get().GetGLFWInstance(), &args.parentWindow);
		nfdresult_t result = NFD_PickFolderU8_With(&outPath, &args);
		if (result == NFD_OKAY)
		{
			m_currentProjectPath = outPath;
			
			NFD_FreePath(outPath);
		}
	}

	ImGui::SetCursorPosX(40);
	if (ImGui::Button("Cancel"))
	{
		m_mode = ProjectManagerMode::List;
	}
	ImGui::SameLine();
	ImGui::BeginDisabled(projectAlreadyExists || m_currentProjectPath.empty() || m_currentProjectName.empty());
	if (ImGui::Button("Create") && !m_currentProjectPath.empty() && !m_currentProjectName.empty())
	{
		if (!std::filesystem::exists(m_currentProjectPath / m_currentProjectName))
		{
			switch (m_mode)
			{
			case ProjectManagerMode::Create:
				CreateProject(m_currentProjectPath / m_currentProjectName);
				break;
			case ProjectManagerMode::CreateFromPackage:
				CreateProjectFromPackage(m_currentProjectPath / m_currentProjectName);
				break;
			}
		}
	}
	ImGui::EndDisabled();

	ImGui::End();
}

bool ProjectManager::LoadProject(std::filesystem::path projectPath)
{
	if (projectPath.extension() == ".solacepkg")
	{
		m_currentProjectName = projectPath.stem().string();
		m_currentProjectPath = projectPath.parent_path();
		m_currentPackageSource = projectPath;
		m_mode = ProjectManagerMode::CreateFromPackage;
		return true;
	}
	if (!std::filesystem::exists(projectPath))
	{
		return false;
	}
	m_currentProjectPath = projectPath.parent_path();

	std::ifstream stream(projectPath);
	std::stringstream sstream;
	sstream << stream.rdbuf();

	JSON data = JSON::parse(sstream.str());

	m_currentProjectAssetsPath = m_currentProjectPath / data["AssetsFolder"].get<std::string>();
	m_currentProjectName = m_currentProjectPath.stem().string();
	auto handle = AssetHandle::from_string(data["StartupScene"].get<std::string>());
	if (handle.has_value())
	{
		m_startupScene = handle.value();
	}

	if (!ExistsInProjectList(projectPath)) m_projectList.push_back(projectPath);
	SerialiseProjectList();
	m_isProjectLoaded = true;
	return true;
}

bool ProjectManager::ShowProjectExistanceWarn(bool show)
{
	if (!show)
	{
		return false;
	}
	
	ImGui::OpenPopup("Project Not Found");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Project Not Found", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("It seems this project no longer exists.");
		ImGui::Separator();

		ImGui::SetItemDefaultFocus();
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x / 2) - 60);
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			return false;
		}
		ImGui::EndPopup();
	}

	return true;
}

void ProjectManager::CreateProject(std::filesystem::path projectPath)
{
	m_currentProjectPath = projectPath;
	m_currentProjectAssetsPath = projectPath / "Assets";
	std::filesystem::create_directory(m_currentProjectPath);
	std::filesystem::create_directory(m_currentProjectAssetsPath);

	JSON json = CreateProjectJson();

	std::filesystem::path projectFile = (m_currentProjectPath / m_currentProjectName).replace_extension(".solaceproj");
	std::ofstream file((m_currentProjectPath / m_currentProjectName).replace_extension(".solaceproj").string());
	if (file.is_open())
	{
		file << json.dump(4); // 4 spaces indentation
		file.close();
	}
	else
	{
		// Handle file opening errors appropriately
		throw std::runtime_error("Unable to open file for writing: " + m_currentProjectPath.string());
	}
	m_projectList.push_back(projectFile);
	SerialiseProjectList();
	m_isProjectLoaded = true;
}

void ProjectManager::CreateProjectFromPackage(std::filesystem::path projectPath)
{
	ZipArchive::Ptr archive = ZipFile::Open(m_currentPackageSource.string());
	
	std::filesystem::path projectFile;
	for (size_t i = 0; i < archive->GetEntriesCount(); ++i)
	{
		auto entry = archive->GetEntry(i);

		std::filesystem::path outputPath = projectPath / entry->GetFullName();

		// Directory entry
		if (entry->IsDirectory())
		{
			std::filesystem::create_directories(outputPath);
			continue;
		}

		// Ensure parent exists
		std::filesystem::create_directories(outputPath.parent_path());

		{
			// Open decompression stream
			std::istream* decompressStream = entry->GetDecompressionStream();

			// Write file
			std::ofstream out(outputPath, std::ios::binary);

			out << decompressStream->rdbuf();
		}

		if (outputPath.extension() == ".solaceproj")
		{
			projectFile = outputPath;
		}
	}
	if (projectFile.empty()) { return; }

	LoadProject(projectFile);
}

JSON ProjectManager::CreateProjectJson()
{
	JSON out;
	out["AssetsFolder"] = "Assets";
	out["StartupScene"] = uuids::to_string(AssetHandle());
	return out;
}

void ProjectManager::SerialiseProjectList()
{
	JSON json;
	// Prevent duplicates
	std::vector<std::filesystem::path> projectsToSerialise(m_projectList);
	m_projectList.clear();
	for (auto& project : projectsToSerialise)
	{
		bool projectNotInList = std::find(m_projectList.begin(), m_projectList.end(), project) == m_projectList.end();
		if (std::filesystem::exists(project) && projectNotInList)
		{
			m_projectList.push_back(project);
		}
	}
	for (auto& project : m_projectList)
	{
		json["Projects"].push_back(project);
	}
	std::ofstream file(Application::GetDataPath() / "ProjectList.json");
	if (file.is_open())
	{
		file << json.dump(4); // 4 spaces indentation
		file.close();
	}
	else
	{
		// Handle file opening errors appropriately
		throw std::runtime_error("Unable to open file for writing: " + (Application::GetDataPath() / "ProjectList.json").string());
	}
}

bool ProjectManager::ExistsInProjectList(std::filesystem::path projectPath)
{
	auto it = std::find(m_projectList.begin(), m_projectList.end(), projectPath);
	return it != m_projectList.end();
}

bool ProjectManager::ProjectEntry(const std::string& projectName, const std::string& projectPath, const ImVec2& size)
{
	ImGui::PushID(projectPath.c_str());
	ImGui::SetCursorPosX(30);
	ImGui::BeginChild("ProjectEntry", size, 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 avail = ImGui::GetContentRegionAvail();

	bool opened = ImGui::Selectable("##select", false, ImGuiSelectableFlags_AllowOverlap, avail);

	ImGui::SetCursorPos(ImVec2(10, 0));	

	ImGui::PushFont(nullptr, 20.0f);
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(projectName.c_str());
	ImGui::PopFont();

	ImVec2 buttonSize = ImVec2(50, 50);
	float buttonMarginRight = 60.0f;
	float prevCursorY = ImGui::GetCursorPosY();
	ImGui::SetCursorPos(ImVec2(avail.x - buttonMarginRight - (buttonSize.x / 2), (avail.y / 2) - (buttonSize.y / 2)));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushFont(nullptr, 24.0f);
	if (ImGui::Button("X", buttonSize))
	{
		m_projectList.erase(std::find(m_projectList.begin(), m_projectList.end(), projectPath));
		SerialiseProjectList();
	}
	ImGui::PopFont();
	ImGui::PopStyleColor();

	ImGui::SetCursorPosY(prevCursorY);
	ImGui::SetCursorPosX(10);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
	ImGui::TextUnformatted(projectPath.c_str());
	ImGui::PopStyleColor();

	ImGui::EndChild();

	ImGui::PopID();

	return opened;
}
