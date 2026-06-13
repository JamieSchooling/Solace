#include "Windows/AssetBrowser.h"

#include "Core/Editor.h"
#include "Core/EditorSystem.h"
#include "Core/Application.h"

#include <fstream>
#include <Assets/MaterialSerialiser.h>
#include <Rendering/Material.h>
#include <Windows/MaterialWindow.h>
#include <Scenes/SceneSerialiser.h>
#include <Assets/AssetCache.h>

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
			AssetRegistry::Get().RegisterNewAsset(target);
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
	FileDropToMoveTarget(m_baseDirectory);

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
		FileDropToMoveTarget(m_baseDirectory / target);
	}

	ImGui::EndChild();

	std::vector<std::filesystem::path> directoryPaths;
	std::vector<std::filesystem::path> filePaths;
	if (!std::filesystem::exists(m_baseDirectory))
	{
		std::filesystem::create_directories(m_baseDirectory);
		m_currentDirectory = m_baseDirectory;
	}
	else if (!std::filesystem::exists(m_currentDirectory))
	{
		m_currentDirectory = m_baseDirectory;
	}
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

	bool itemContextMenuOpened = false;

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
			if (ImGui::ImageButton(directoryPath.string().c_str(), (ImTextureRef)m_directoryIcon->GetID(), m_thumbnailSize))
			{
				// This will set selected *asset* rather than selected entity to display inspector info
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_currentDirectory /= directoryPath.filename();
			}
			FileDropToMoveTarget(directoryPath);
			if (ImGui::BeginDragDropSource())
			{
				const auto itemPath = directoryPath.c_str();
				const size_t pathSize = (wcslen(itemPath) + 1) * sizeof(std::filesystem::path::value_type);
				ImGui::SetDragDropPayload("Directory_Item", itemPath, pathSize, ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}
			bool shouldDelete = false;
			if (ImGui::BeginPopupContextItem())
			{
				itemContextMenuOpened = true;
				if (ImGui::MenuItem("Delete"))
				{
					shouldDelete = true;
				}
				ImGui::EndPopup();
			}
			if (Delete(directoryPath, shouldDelete))
			{
				std::filesystem::remove_all(directoryPath);

				AssetRegistry::Get().DeleteDirectory(directoryPath);
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
					auto path = std::filesystem::relative(filePath, m_baseDirectory);
					MaterialWindow::Open(AssetRegistry::Get().GetHandle(path));
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
					EditorSystem::Get().SetCurrentlyOpenScene(filePath);
					auto path = std::filesystem::relative(filePath, m_baseDirectory);
					Editor::SetStartupScene(AssetRegistry::Get().GetHandle(path));
				}
			}

			if (ImGui::BeginDragDropSource())
			{
				auto path = std::filesystem::relative(filePath, m_baseDirectory);
				AssetHandle handle = AssetRegistry::Get().GetHandle(path);

				ImGui::SetDragDropPayload("Asset_Item", &handle, sizeof(AssetHandle));

				ImGui::EndDragDropSource();
			}

			bool shouldDelete = false;
			if (ImGui::BeginPopupContextItem())
			{
				itemContextMenuOpened = true;
				if (ImGui::MenuItem("Delete"))
				{
					shouldDelete = true;
				}
				ImGui::EndPopup();
			}
			if (Delete(filePath, shouldDelete))
			{
				std::filesystem::remove(filePath);

				auto path = std::filesystem::relative(filePath, m_baseDirectory);
				AssetHandle handle = AssetRegistry::Get().GetHandle(path);
				AssetRegistry::Get().DeleteAsset(handle);
				Assimp::Importer importer;
				if (filePath.extension() == ".mat")
				{
					AssetCache::Delete<Material>(handle);
				}
				else if(importer.IsExtensionSupported(filePath.extension().string()))
				{
					AssetCache::Delete<Mesh>(handle);
				}
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

	if (!itemContextMenuOpened && ImGui::BeginPopupContextItem())
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
				AssetRegistry::Get().RegisterNewAsset(path);
				StartFilenameEdit(path);
			}
			if (ImGui::MenuItem("Material"))
			{
				std::shared_ptr<Shader> shader = std::make_shared<Shader>("./resources/shaders/Vertex.glsl", "./resources/shaders/Fragment.glsl");
				std::shared_ptr<Material> material = std::make_shared<Material>(shader, MaterialInitFlags_InitWithDefaultValues);
				material->SetValue("u_prop_colour", glm::vec3(1.0));
				material->SetValue("u_prop_specularAmount", 0.5f);
				MaterialSerialiser ms;
				auto path = m_currentDirectory / "Material.mat";
				AppendDuplicateCount(path);
				ms.SerialiseTo(material, path);
				AssetRegistry::Get().RegisterNewAsset(path);
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
	std::string filename = m_currentEditFilepathModified.stem().string();
	std::string extension = m_currentEditFilepathModified.extension().string();
	if (ImGui::InputText("##Filename", &filename, ImGuiInputTextFlags_AutoSelectAll))
	{
		m_currentEditFilepathModified = m_currentDirectory / (filename + extension);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Enter))
	{
		m_editingFilename = false;
		AppendDuplicateCount(m_currentEditFilepathModified);
		std::filesystem::rename(path, m_currentEditFilepathModified);
		if (std::filesystem::is_directory(m_currentEditFilepathModified))
		{
			AssetRegistry::Get().MoveDirectory(m_currentEditFilepath, m_currentEditFilepathModified);
			return; 
		}

		AssetHandle handle = AssetRegistry::Get().GetHandle(std::filesystem::relative(m_currentEditFilepath, m_baseDirectory));
		if (!handle.is_nil())
		{
			AssetRegistry::Get().MoveAsset(handle, m_currentEditFilepathModified);
		}
		else
		{
			AssetRegistry::Get().RegisterNewAsset(m_currentEditFilepathModified);
		}
	}
}

void AssetBrowser::StartFilenameEdit(const std::filesystem::path& path)
{
	m_editingFilename = true;
	m_currentEditFilepath = path;
	m_currentEditFilepathModified = path;
}

void AssetBrowser::FileDropToMoveTarget(const std::filesystem::path& targetDirectory)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset_Item"))
		{
			AssetHandle handle = *(AssetHandle*)payload->Data;
			std::filesystem::path path = AssetRegistry::Get().GetFullPath(handle);
			std::filesystem::path target = targetDirectory / path.filename();
			AppendDuplicateCount(target, targetDirectory);
			std::filesystem::rename(path, target);
			AssetRegistry::Get().MoveAsset(handle, target);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Directory_Item"))
		{
			const wchar_t* pathString = (const wchar_t*)payload->Data;
			std::filesystem::path path = pathString;
			if (targetDirectory.string().starts_with(path.string()))
			{
				return;
			}
			std::filesystem::path target = targetDirectory / path.filename();
			AppendDuplicateCount(target, targetDirectory);
			std::filesystem::rename(path, target);
			AssetRegistry::Get().MoveDirectory(path, target);
		}
		ImGui::EndDragDropTarget();
	}
}

bool AssetBrowser::Delete(std::filesystem::path target, bool show)
{
	ImGui::PushID(target.string().c_str());
	if (show)
	{
		ImGui::OpenPopup("Delete");
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Delete", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are you sure?\nThis operation cannot be undone.");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) 
		{ 
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			ImGui::PopID();
			return true; 
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) 
		{ 
			ImGui::CloseCurrentPopup(); 
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();

	return false;
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

void AssetBrowser::AppendDuplicateCount(std::filesystem::path& target, const std::filesystem::path& targetDir)
{
	if (std::filesystem::exists(target))
	{
		const auto stem = target.stem().string();
		const auto extension = target.extension().string();

		int index = 1;
		do
		{
			target = targetDir /
				std::format("{} {}{}", stem, index, extension);

			++index;
		} while (std::filesystem::exists(target));
	}
}
