#pragma once

#include "Menu/MenuRegistry.h"

class FileMenu
{
public:
	static void Open();
	MENU_ITEM("File/Open", Open)

	static void SaveAs();
	MENU_ITEM("File/Save As...", SaveAs)
};