#include "Menu/LayoutMenu.h"

void LayoutMenu::Default()
{
	EditorSystem::Get().SetLayout(LayoutOption::Default);
}

void LayoutMenu::Active()
{
	EditorSystem::Get().SetLayout(LayoutOption::Active);
}
