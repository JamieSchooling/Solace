#include "Menu/FileMenu.h"
#include <nfd.h>
#include <iostream>
#include <Scenes/Scene.h>
#include <Scenes/SceneSerialiser.h>
#include <Scenes/SceneSystem.h>

void FileMenu::Open()
{
	nfdchar_t* outPath = nullptr;
	NFD_OpenDialog("solace", nullptr, &outPath);
	if (outPath)
	{
		std::cout << outPath << std::endl;
		Scene scene;
		SceneSerialiser serialiser(scene);
		serialiser.DeserialiseFrom(outPath);
		SceneSystem::Get().LoadScene(scene);
	}
}

void FileMenu::SaveAs()
{
	nfdchar_t* outPath = nullptr;
	NFD_SaveDialog("solace", nullptr, &outPath);
	if (outPath)
	{
		std::cout << outPath << std::endl;
		SceneSerialiser serialiser(SceneSystem::Get().GetActiveScene());
		serialiser.SerialiseTo(outPath);
	}
}
