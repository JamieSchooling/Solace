#pragma once

#include <functional>

using EditorAction = std::function<void()>;

struct UndoCommand
{
	EditorAction UndoAction;
	EditorAction Action;
};

struct RedoCommand
{
	EditorAction RedoAction;
	EditorAction UndoAction;
};