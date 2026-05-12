#pragma once

#include "Menu/MenuRegistry.h"

class FileMenu
{
public:
	static void Open();
	MENU_ITEM("File/Open", 100, Open)

	static void SaveAs();
	MENU_ITEM("File/Save As...", 110, SaveAs)
};