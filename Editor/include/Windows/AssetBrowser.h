#pragma once

#include <filesystem>

#include "Windows/EditorWindow.h"
#include "Menu/MenuRegistry.h"
#include "Inspectors/ComponentInspector.h"

#include <Rendering/Texture.h>

class AssetBrowser : public EditorWindow
{
public:
	AssetBrowser() : EditorWindow("Asset Browser") {}

	static void Open();
	MENU_ITEM("Window/General/Asset Browser", 420, Open);

	void Initialise(Scene& scene) override;
	void OnEvent(Event& e) override;
protected:
	void DrawContent(entt::entity& selected, Scene& scene) override;
private:
	std::filesystem::path m_baseDirectory;
	std::filesystem::path m_currentDirectory;
	std::vector<std::filesystem::path> m_droppedFiles;
	ImVec2 thumbnailSize = { 64.0f, 64.0f };
	std::unique_ptr<Texture> m_directoryIcon;
	std::unique_ptr<Texture> m_fileIcon;
};