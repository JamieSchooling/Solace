#pragma once

#include "Application.h"

int main()
{
	Application* app = CreateApplication();
	app->ExecuteLifecycle();
	delete app;

	return 0;
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return main();
}
#endif
