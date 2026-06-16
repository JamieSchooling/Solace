#pragma once

#include "MenuRegistry.h"

#include "Core/Undo.h"

class EditMenu
{
public:
	static void Undo();
	MENU_ITEM_DISABLABLE("Edit/Undo", 200, Undo, "Ctrl + Z", []() { return UndoSystem::UndoCount() <= 0; })

	static void Redo();
	MENU_ITEM_DISABLABLE("Edit/Redo", 210, Redo, "Ctrl + Y", []() { return UndoSystem::RedoCount() <= 0; })
};