#pragma once

#include <imgui.h>

class Fonts
{
public:
	static void Load();

	static ImFont* Default;
	static ImFont* SemiBold;
	static ImFont* Bold;
};