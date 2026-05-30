#pragma once

class ModuleLoader
{
public:
	static void* Load(const char* modulePath);
	static void* GetSymbol(void* lib, const char* symbolName);
};