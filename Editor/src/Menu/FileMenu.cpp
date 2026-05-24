#include "Menu/FileMenu.h"
#include <nfd.h>
#include <nfd_glfw3.h>
#include <iostream>
#include <Scenes/Scene.h>
#include <Scenes/SceneSerialiser.h>
#include <Scenes/SceneSystem.h>
#include <Core/Application.h>

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
		std::cout << outPath << std::endl;
		Scene scene;
		SceneSerialiser serialiser(scene);
		serialiser.DeserialiseFrom(outPath);
		SceneSystem::Get().LoadScene(scene);
		NFD_FreePath(outPath);
	}
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
		std::cout << outPath << std::endl;
		SceneSerialiser serialiser(SceneSystem::Get().GetActiveScene());
		serialiser.SerialiseTo(outPath);
		NFD_FreePath(outPath);
	}
}
