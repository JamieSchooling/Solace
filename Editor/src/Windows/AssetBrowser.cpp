#include "Windows/AssetBrowser.h"

#include "Core/Editor.h"
#include "Core/EditorSystem.h"
#include "Core/Application.h"

#include <fstream>
#include <Assets/MaterialSerialiser.h>
#include <Rendering/Material.h>
#include <Windows/MaterialWindow.h>
#include <Scenes/SceneSerialiser.h>

void AssetBrowser::Open()
{
	EditorSystem::Get().OpenWindow<AssetBrowser>();
}

void AssetBrowser::Initialise()
{
	m_baseDirectory = Editor::ProjectAssetsPath();
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
			AppendDuplicateCount(target);

			std::ifstream src(file, std::ios::binary);
			std::ofstream dst(target, std::ios::binary);

			dst << src.rdbuf();
		}

		m_droppedFiles.clear();
	}

	ImGui::BeginChild("##Toolbar", ImVec2(0, 22), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration);

	std::filesystem::path relative =
		std::filesystem::relative(m_currentDirectory, m_baseDirectory);

	std::vector<std::filesystem::path> parts(relative.begin(), relative.end());

	std::filesystem::path target;

	if (ImGui::SmallButton("Assets"))
	{
		m_currentDirectory = m_baseDirectory;
	}

	for (int i = 0; i < parts.size(); ++i)
	{
		if (parts[i] == ".") { continue; }

		ImGui::SameLine();
		ImGui::TextUnformatted(">");

		ImGui::SameLine();

		target /= parts[i];

		if (ImGui::SmallButton(parts[i].string().c_str()))
		{
			m_currentDirectory = m_baseDirectory / target;
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

	bool startedFilenameEditThisFrame = false;

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

			if (m_editingFilename && directoryPath == m_currentEditFilepath)
			{
				DrawFilenameEdit(directoryPath, cellWidth);
			}
			else
			{
				DrawTruncatedPath(directoryPath.filename(), cellWidth);
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				startedFilenameEditThisFrame = true;
				StartFilenameEdit(directoryPath);
			}

			ImGui::TableNextColumn();
		}

		for (const auto& filePath : filePaths)
		{
			if (ImGui::ImageButton(filePath.filename().string().c_str(), m_fileIcon->GetID(), m_thumbnailSize))
			{
				// This will set selected *asset* rather than selected entity to display inspector info
				if (filePath.extension() == ".mat")
				{
					MaterialWindow::Open(filePath);
				}
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (filePath.extension() == ".solace")
				{
					Scene scene;
					SceneSerialiser serialiser(scene);
					serialiser.DeserialiseFrom(filePath);
					SceneSystem::Get().LoadScene(scene);
				}
			}

			if (ImGui::BeginDragDropSource())
			{
				const auto itemPath = filePath.c_str();
				const size_t pathSize = (wcslen(itemPath) + 1) * sizeof(std::filesystem::path::value_type);
				ImGui::SetDragDropPayload("Asset_Item", itemPath, pathSize, ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			if (m_editingFilename && filePath == m_currentEditFilepath)
			{
				DrawFilenameEdit(filePath, cellWidth);
			}
			else
			{
				DrawTruncatedPath(filePath.filename(), cellWidth);
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				startedFilenameEditThisFrame = true;
				StartFilenameEdit(filePath);
			}
			
			ImGui::TableNextColumn();
		}
		ImGui::EndTable();
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && (ImGui::IsWindowHovered() || ImGui::IsItemHovered()) && !startedFilenameEditThisFrame)
	{
		m_editingFilename = false;
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				auto path = m_currentDirectory / "New Folder";
				AppendDuplicateCount(path);
				std::filesystem::create_directory(path);
				StartFilenameEdit(path);
			}
			if (ImGui::MenuItem("Scene"))
			{
				Scene scene = Scene::CreateDefault();
				SceneSerialiser serialiser(scene);
				auto path = m_currentDirectory / "New Scene.solace";
				AppendDuplicateCount(path);
				serialiser.SerialiseTo(path);
				SceneSystem::Get().LoadScene(scene);
				StartFilenameEdit(path);
			}
			if (ImGui::MenuItem("Material"))
			{
				std::shared_ptr<Shader> shader = std::make_shared<Shader>("./resources/shaders/Vertex.glsl", "./resources/shaders/Fragment.glsl");
				std::shared_ptr<Material> material = std::make_shared<Material>(shader, InitWithDefaultValues);
				MaterialSerialiser ms;
				auto path = m_currentDirectory / "Material.mat";
				AppendDuplicateCount(path);
				ms.SerialiseTo(material, path);

				StartFilenameEdit(path);
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
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

void AssetBrowser::DrawFilenameEdit(const std::filesystem::path& path, float maxWidth)
{
	ImGui::SetNextItemWidth(maxWidth);
	std::string filename = m_currentEditFilepathModified.filename().string();
	if (ImGui::InputText("##Filename", &filename, ImGuiInputTextFlags_AutoSelectAll))
	{
		m_currentEditFilepathModified = m_currentDirectory / filename;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Enter))
	{
		std::filesystem::rename(path, m_currentEditFilepathModified);
		m_editingFilename = false;
	}
}

void AssetBrowser::StartFilenameEdit(const std::filesystem::path& path)
{
	m_editingFilename = true;
	m_currentEditFilepath = path;
	m_currentEditFilepathModified = path;
}

void AssetBrowser::AppendDuplicateCount(std::filesystem::path& target)
{
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
}
