#include "Windows/AssetBrowser.h"

#include "Core/EditorSystem.h"
#include "Core/Application.h"

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

void AssetBrowser::DrawContent(entt::entity& selected, Scene& scene)
{
	if (m_currentDirectory != m_baseDirectory)
	{
		if (ImGui::Button("<-"))
		{
			m_currentDirectory = m_currentDirectory.parent_path();
		}
	}

	for (auto& directoryEntry : std::filesystem::directory_iterator(m_currentDirectory))
	{
		std::filesystem::path relativePath = std::filesystem::relative(directoryEntry.path(), m_baseDirectory);
		if (directoryEntry.is_directory())
		{
			if (ImGui::Button(relativePath.string().c_str()))
			{
				// This will set selected *asset* rather than selected entity to display inspector info
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_currentDirectory /= relativePath.filename();
			}
		}
		else
		{
			if (ImGui::Button(relativePath.filename().string().c_str())) {}
		}
	}
}
