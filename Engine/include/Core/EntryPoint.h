#pragma once

#include "Application.h"

int main()
{
	Application* app = CreateApplication();
	app->ExecuteLifecycle();
	delete app;

	return 0;
}
