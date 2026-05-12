#pragma once

#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <unordered_map>

struct MenuNode
{
	std::string Name;
	std::function<void()> Action;
	std::unordered_map<std::string, std::unique_ptr<MenuNode>> Children;
};

class MenuRegistry
{
public:
	static MenuNode& Root()
	{
		static MenuNode root;
		return root;
	}

	static void Register(const char* path, std::function<void()> action)
	{
		std::stringstream ss(path);

		std::string segment;

		MenuNode* current = &Root();

		while (std::getline(ss, segment, '/'))
		{
			auto& child = current->Children[segment];

			if (!child)
			{
				child = std::make_unique<MenuNode>();
				child->Name = segment;
			}

			current = child.get();
		}

		current->Action = std::move(action);
	}

};

#define MENU_ITEM(path, action) \
	static inline bool _reg_item_##action = []() \
	{ \
		MenuRegistry::Register(path, action); \
		return true; \
	}();