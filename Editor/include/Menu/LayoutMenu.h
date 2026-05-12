#pragma once

#include "Menu/MenuRegistry.h"

#include "Core/EditorSystem.h"

class LayoutMenu
{
public:
	static void Default();
	MENU_ITEM_SELECTION("Layout/Default", 300, Default, []() { return EditorSystem::Get().GetLayout() == LayoutOption::Default; })
	static void Active();
	MENU_ITEM_SELECTION("Layout/Active", 310, Active, []() { return EditorSystem::Get().GetLayout() == LayoutOption::Active; })
};