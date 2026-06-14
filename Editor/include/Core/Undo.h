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

class UndoSystem
{
public:
	static void AddUndoCommand(UndoCommand command);
	static void Undo();
	static size_t UndoCount();
	static void Redo();
	static size_t RedoCount();
private:
	static inline std::vector<UndoCommand> m_undoStack;
	static inline std::vector<RedoCommand> m_redoStack;
};