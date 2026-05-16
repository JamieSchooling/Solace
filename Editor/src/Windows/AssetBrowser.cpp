#include "Windows/AssetBrowser.h"

#include "Core/EditorSystem.h"
#include "Core/Application.h"

#include <fstream>

void AssetBrowser::Open()
{
	EditorSystem::Get().OpenWindow<AssetBrowser>();
}

void AssetBrowser::Initialise(Scene& scene)
{
	// TODO: Change this once projects are eventually implemented
	m_baseDirectory = Application::GetResourcePath();
	m_currentDirectory = m_baseDirectory;
}

void AssetBrowser::OnEvent(Event& e)
{
	if (e.Type == EventType::WindowFileDropped)
	{
		m_droppedFiles.clear();
		for (int i = 0; i < e.WindowFileDroppedArgs.Count; i++)
		{
			m_droppedFiles.emplace_back(e.WindowFileDroppedArgs.Paths[i]);
		}
	}
}

void AssetBrowser::DrawContent(entt::entity& selected, Scene& scene)
{
	if (ImGui::IsWindowHovered() && !m_droppedFiles.empty())
	{
		for (const auto& file : m_droppedFiles)
		{
			auto target = m_currentDirectory / file.filename();
			if (std::filesystem::exists(target))
			{
				const auto stem = target.stem().string();
				const auto extension = target.extension().string();

				int index = 1;
				do
				{
					target = m_currentDirectory /
						std::format("{} {}{}", stem, index, extension);

					++index;
				} while (std::filesystem::exists(target));
			}
			std::ifstream src(file, std::ios::binary);
			std::ofstream dst(target, std::ios::binary);

			dst << src.rdbuf();
		}

		m_droppedFiles.clear();
	}

	if (m_currentDirectory != m_baseDirectory)
	{
		if (ImGui::ArrowButton("##Back", ImGuiDir_Left))
		{
			m_currentDirectory = m_currentDirectory.parent_path();
		}
	}

	std::vector<std::filesystem::path> directoryPaths;
	std::vector<std::filesystem::path> filePaths;
	for (auto& directoryEntry : std::filesystem::directory_iterator(m_currentDirectory))
	{
		if (directoryEntry.is_directory())
		{
			directoryPaths.push_back(directoryEntry.path());
		}
		else
		{
			filePaths.push_back(directoryEntry.path());
		}
	}

	for (const auto& directoryPath : directoryPaths)
	{
		if (ImGui::Button(directoryPath.filename().string().c_str()))
		{
			// This will set selected *asset* rather than selected entity to display inspector info
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			m_currentDirectory /= directoryPath.filename();
		}
	}

	for (const auto& filePath : filePaths)
	{
		if (ImGui::Button(filePath.filename().string().c_str()))
		{
			// This will set selected *asset* rather than selected entity to display inspector info
		}

		if (ImGui::BeginDragDropSource())
		{
			const auto itemPath = filePath.c_str();
			const size_t pathSize = (wcslen(itemPath) + 1) * sizeof(std::filesystem::path::value_type);
			ImGui::SetDragDropPayload("Asset_Item", itemPath, pathSize, ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}
	}
}
