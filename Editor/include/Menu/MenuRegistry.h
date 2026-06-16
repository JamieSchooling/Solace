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
	std::string Shortcut;
	uint16_t Priority = UINT16_MAX;
	std::function<void()> Action;
	std::function<bool()> SelectedCondition = nullptr;
	std::function<bool()> DisabledCondition = nullptr;
	std::vector<std::unique_ptr<MenuNode>> Children;
};

enum class MenuConditionType
{
	Select,
	Disable
};

class MenuRegistry
{
public:
	static MenuNode& Root()
	{
		static MenuNode root;
		return root;
	}

	static void Register(const char* path, uint16_t priority, std::function<void()> action, std::string shortcut, 
						 std::function<bool()> condition = nullptr, MenuConditionType conditionType = MenuConditionType::Select)
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
				child->Shortcut = shortcut;
			}

			child->Priority = std::min(current->Priority, priority);
			current = child;
		}

		current->Action = std::move(action);
		switch (conditionType)
		{
		case MenuConditionType::Select:
			current->SelectedCondition = condition;
			break;
		case MenuConditionType::Disable:
			current->DisabledCondition = condition;
			break;
		}

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

#define MENU_ITEM(path, priority, action, shortcut) \
	static inline bool _reg_item_##action = []() \
	{ \
		MenuRegistry::Register(path, priority, action, shortcut); \
		return true; \
	}();

#define MENU_ITEM_SELECTION(path, priority, action, selectedCondition) \
	static inline bool _reg_item_##action = []() \
	{ \
		MenuRegistry::Register(path, priority, action, "", selectedCondition, MenuConditionType::Select); \
		return true; \
	}();

#define MENU_ITEM_DISABLABLE(path, priority, action, shortcut, disableCondition) \
	static inline bool _reg_item_##action = []() \
	{ \
		MenuRegistry::Register(path, priority, action, shortcut, disableCondition, MenuConditionType::Disable); \
		return true; \
	}();

