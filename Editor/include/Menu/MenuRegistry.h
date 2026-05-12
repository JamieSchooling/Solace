#pragma once

#include <algorithm>
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <vector>

struct MenuNode
{
	std::string Name; 
	uint16_t Priority = UINT16_MAX;
	std::function<void()> Action;
	std::vector<std::unique_ptr<MenuNode>> Children;
};

class MenuRegistry
{
public:
	static MenuNode& Root()
	{
		static MenuNode root;
		return root;
	}

	static void Register(const char* path, uint16_t priority, std::function<void()> action)
	{
		std::stringstream ss(path);

		std::string segment;

		MenuNode* current = &Root();

		while (std::getline(ss, segment, '/'))
		{
			MenuNode* child = GetChild(*current, segment);

			if (!child)
			{
				current->Children.push_back(std::make_unique<MenuNode>());
				child = current->Children.back().get();
				child->Name = segment;
			}

			child->Priority = std::min(current->Priority, priority);
			current = child;
		}

		current->Action = std::move(action);

		Sort(Root());
	}

private:
	static MenuNode* GetChild(MenuNode& node, std::string_view name)
	{
		for (auto& child : node.Children)
		{
			if (child->Name == name)
			{
				return child.get();
			}
		}

		return nullptr;
	}

	static void Sort(MenuNode& node)
	{
		std::sort(node.Children.begin(), node.Children.end(),
		[](const std::unique_ptr<MenuNode>& a, const std::unique_ptr<MenuNode>& b)
		{
			if (a->Priority == b->Priority)
			{
				return a->Name < b->Name;
			}

			return a->Priority < b->Priority;
		});

		for (auto&& child : node.Children)
		{
			Sort(*child);
		}
	}

};

#define MENU_ITEM(path, priority, action) \
	static inline bool _reg_item_##action = []() \
	{ \
		MenuRegistry::Register(path, priority, action); \
		return true; \
	}();

