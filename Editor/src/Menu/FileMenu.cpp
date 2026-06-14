#include "Menu/FileMenu.h"
#include <Core/Editor.h>
#include <nfd.h>
#include <nfd_glfw3.h>
#include <iostream>
#include <Scenes/Scene.h>
#include <Scenes/SceneSerialiser.h>
#include <Scenes/SceneSystem.h>
#include <Core/EditorSystem.h>
#include <Core/Application.h>
#include <ZipLib/ZipFile.h>
#include <ZipLib/ZipArchiveEntry.h>
#include <fstream>

void FileMenu::Open()
{
	nfdchar_t* outPath = nullptr;
	nfdopendialogu8args_t args = { 0 }; 
	nfdu8filteritem_t filters[1] = { { "Scenes", "solace" } };
	args.filterList = filters;
	args.filterCount = 1;
	NFD_GetNativeWindowFromGLFWWindow(Window::Get().GetGLFWInstance(), &args.parentWindow); 
	nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
	if (result == NFD_OKAY)
	{
		Scene scene;
		SceneSerialiser serialiser(scene);
		serialiser.DeserialiseFrom(outPath);
		SceneSystem::Get().LoadScene(scene);
		EditorSystem::Get().SetCurrentlyOpenScene(outPath);
		auto path = std::filesystem::relative(outPath, Editor::ProjectAssetsPath());
		Editor::SetStartupScene(AssetRegistry::Get().GetHandle(path));
		NFD_FreePath(outPath);
		EditorSystem::Get().SetSceneDirty(false);
	}
}

void FileMenu::Save()
{
	std::filesystem::path path = EditorSystem::Get().GetCurrentlyOpenScene();
	if (path.empty() || !std::filesystem::exists(path))
	{
		SaveAs();
		return;
	}

	SceneSerialiser serialiser(SceneSystem::Get().GetActiveScene());
	serialiser.SerialiseTo(path);
	EditorSystem::Get().SetSceneDirty(false);
}

void FileMenu::SaveAs()
{
	nfdchar_t* outPath = nullptr;
	nfdsavedialogu8args_t args = { 0 };
	nfdu8filteritem_t filters[1] = { { "Scenes", "solace" } };
	args.filterList = filters;
	args.filterCount = 1;
	args.defaultName = "Untitled";
	NFD_GetNativeWindowFromGLFWWindow(Window::Get().GetGLFWInstance(), &args.parentWindow);
	nfdresult_t result = NFD_SaveDialogU8_With(&outPath, &args);
	if (result == NFD_OKAY)
	{
		SceneSerialiser serialiser(SceneSystem::Get().GetActiveScene());
		std::filesystem::path scenePath(outPath);
		serialiser.SerialiseTo(scenePath);
		AssetRegistry::Get().RegisterNewAsset(scenePath);
		EditorSystem::Get().SetCurrentlyOpenScene(outPath);
		auto path = std::filesystem::relative(outPath, Editor::ProjectAssetsPath());
		Editor::SetStartupScene(AssetRegistry::Get().GetHandle(path));
		NFD_FreePath(outPath);
		EditorSystem::Get().SetSceneDirty(false);
	}
}

void FileMenu::PackageProject()
{
	nfdchar_t* outPath = nullptr;
	nfdsavedialogu8args_t args = { 0 };
	nfdu8filteritem_t filters[1] = { { "Solace Package", "solacepkg" } };
	args.filterList = filters;
	args.filterCount = 1;
	std::string filename = Editor::ProjectDirectoryPath().filename().string();
	args.defaultName = filename.c_str();
	NFD_GetNativeWindowFromGLFWWindow(Window::Get().GetGLFWInstance(), &args.parentWindow);
	nfdresult_t result = NFD_SaveDialogU8_With(&outPath, &args);
	if (result == NFD_OKAY)
	{
		auto archive = ZipFile::Open(outPath);

		const auto root = Editor::ProjectDirectoryPath();

		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(root))
		{
			auto relative = std::filesystem::relative(dirEntry.path(), root);

			std::string archivePath = relative.generic_string();

			if (dirEntry.is_directory())
			{
				archivePath += '/';

				auto entry = archive->CreateEntry(archivePath);
				continue;
			}

			auto entry = archive->CreateEntry(archivePath);
			assert(entry != nullptr);

			std::ifstream contentStream(dirEntry.path(), std::ios::binary);

			entry->SetCompressionStream(
				contentStream,
				ZipArchiveEntry::CompressionLevel::Default,
				ZipArchiveEntry::CompressionMethod::Deflate,
				ZipArchiveEntry::CompressionMode::Immediate
			);
		}

		ZipFile::SaveAndClose(archive, outPath);
		NFD_FreePath(outPath);
	}

	
}
