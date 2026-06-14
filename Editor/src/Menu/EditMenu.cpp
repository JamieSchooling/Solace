#include "Menu/EditMenu.h"

void EditMenu::Undo()
{
	UndoSystem::Undo();
}

void EditMenu::Redo()
{
	UndoSystem::Redo();
}
