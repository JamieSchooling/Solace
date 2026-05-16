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

	m_directoryIcon = std::make_unique<Texture>(Application::GetResourcePath() / "Icons" / "DirectoryIcon.png");
	m_fileIcon = std::make_unique<Texture>(Application::GetResourcePath() / "Icons" / "FileIcon.png");
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
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

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

	ImGui::BeginChild("##Toolbar", ImVec2(0, 22), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration);

	std::filesystem::path full = m_currentDirectory.lexically_normal();
	std::vector<std::filesystem::path> parts(full.begin(), full.end());
	std::filesystem::path target;
	for (int i = 0; i < parts.size(); ++i)
	{
		if (i > 0) { ImGui::SameLine(); }

		target /= parts[i];

		if (ImGui::SmallButton(parts[i].string().c_str()))
		{
			m_currentDirectory = target;
		}
		if (i < parts.size() - 1)
		{
			ImGui::SameLine();
			ImGui::Text(">");
		}
	}

	ImGui::EndChild();

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

	ImGuiStyle& style = ImGui::GetStyle();
	float cellWidth = m_thumbnailSize.x + style.CellPadding.x * 2.0f;
	float availableWidth = ImGui::GetContentRegionAvail().x;
	availableWidth -= style.ScrollbarSize;

	int columnCount = availableWidth / cellWidth;
	columnCount = std::max(columnCount, 1);
	if (ImGui::BeginTable("##Files", columnCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY, {0.0f, ImGui::GetContentRegionAvail().y - 44.0f}))
	{
		for (int i = 0; i < columnCount; ++i)
		{
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, cellWidth);
		}

		ImGui::TableNextColumn();

		for (const auto& directoryPath : directoryPaths)
		{
			if (ImGui::ImageButton(directoryPath.filename().string().c_str(), (ImTextureRef)m_directoryIcon->GetID(), m_thumbnailSize))
			{
				// This will set selected *asset* rather than selected entity to display inspector info
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_currentDirectory /= directoryPath.filename();
			}

			DrawTruncatedPath(directoryPath.filename(), cellWidth);

			ImGui::TableNextColumn();
		}

		for (const auto& filePath : filePaths)
		{
			if (ImGui::ImageButton(filePath.filename().string().c_str(), m_fileIcon->GetID(), m_thumbnailSize))
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

			DrawTruncatedPath(filePath.filename(), cellWidth);
			
			ImGui::TableNextColumn();
		}
		ImGui::EndTable();
	}

	if (ImGui::SliderFloat("Thumnail Size", &m_thumbnailSize.x, 10.0f, 512.0f))
	{
		m_thumbnailSize.y = m_thumbnailSize.x;
	}

	ImGui::PopStyleColor();
}

void AssetBrowser::DrawTruncatedPath(const std::filesystem::path& path, float maxWidth)
{
	std::string text = path.string();

	while (!text.empty() && ImGui::CalcTextSize((text + "...").c_str()).x > maxWidth)
	{
		text.pop_back();
	}

	if (text != path.string())
		text += "...";

	ImGui::TextUnformatted(text.c_str());
}
