#pragma once

#include "Application.h"

int main()
{
	Application* app = CreateApplication();
	app->Start();
	delete app;

	return 0;
}
