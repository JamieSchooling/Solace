#pragma once

#define NOMINMAX

#include "Application.h"

int Entry(std::vector<std::string> args)
{
	Application* app = CreateApplication();
	app->ExecuteLifecycle(args);
	delete app;

	return 0;
}

int main(int argc, char** argv)
{
	std::vector<std::string> args;
	args.reserve(argc);
	for (int i = 0; i < argc; ++i)
	{
		args.push_back(argv[i]);
	}
	return Entry(args);
}

#ifdef _WIN32
#include <windows.h>

static std::string WideToUtf8(const wchar_t* w)
{
	if (!w) return {};

	int size = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);

	if (size <= 0)
	{
		return {};
	}

	std::string out(size - 1, 0);

	WideCharToMultiByte(CP_UTF8, 0, w, -1, out.data(), size, nullptr, nullptr);

	return out;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int argc = 0;
	wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
	std::vector<std::string> args;
	args.reserve(argc);

	for (int i = 0; i < argc; ++i)
	{
		args.push_back(WideToUtf8(wargv[i]));
	}
	return Entry(args);
}
#endif
