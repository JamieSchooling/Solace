#pragma once

#include "Menu/MenuRegistry.h"

class FileMenu
{
public:
	static void Open();
	MENU_ITEM("File/Open", 100, Open)

	static void Save();
	MENU_ITEM("File/Save", 110, Save)

	static void SaveAs();
	MENU_ITEM("File/Save As...", 120, SaveAs)

	static void PackageProject();
	MENU_ITEM("File/Package Project...", 130, PackageProject)
};