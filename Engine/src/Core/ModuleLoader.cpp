#include "Core/ModuleLoader.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <string>

void* ModuleLoader::Load(const char* modulePath)
{
	std::string filename(modulePath);

#ifdef _WIN32
	filename += ".dll";
	HMODULE module = LoadLibraryA(filename.c_str());
	return reinterpret_cast<void*>(module);
#else
	#if defined(__APPLE__)
	filename += ".dylib";
	#else
	filename += ".so";
	#endif
	void* handle = dlopen(filename.c_str(), RTLD_LAZY);
	return handle;
#endif
}

void* ModuleLoader::GetSymbol(void* lib, const char* symbolName)
{
	if (!lib) return nullptr;

#ifdef _WIN32
	HMODULE module = reinterpret_cast<HMODULE>(lib);
	return reinterpret_cast<void*>(GetProcAddress(module, symbolName));
#else
	return dlsym(lib, symbolName);
#endif
}
