#pragma once
#include <entt/entt.hpp>

#include <vector>
#include <memory>

class InputSystem;

class ISystem
{
public:
	virtual void Initialise(entt::registry& registry, InputSystem* inputSystem) {};
	virtual void Update(entt::registry& registry, float deltaTime, InputSystem* inputSystem) = 0;
};

struct ModuleSystems
{
	std::vector<std::shared_ptr<ISystem>> Systems;
};

template<typename System, typename = std::enable_if_t<std::is_base_of_v<ISystem, System>>>
std::shared_ptr<ISystem> CreateSystem()
{
	return std::make_shared<System>();
}

inline ModuleSystems* GetModuleSystems()
{
	static ModuleSystems module;
	return &module;
}

#define REGISTER_SYSTEM(System) \
	static bool _reg_##System = []() \
	{ \
		GetModuleSystems()->Systems.push_back(CreateSystem<System>()); \
		return true; \
	}();