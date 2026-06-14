#include "Core/Undo.h"

void UndoSystem::AddUndoCommand(UndoCommand command)
{
	m_redoStack.clear();
	m_undoStack.push_back(command);
}

void UndoSystem::Undo()
{
	UndoCommand undo = m_undoStack.back();
	m_undoStack.pop_back();
	undo.UndoAction();

	RedoCommand redo;
	redo.RedoAction = undo.Action;
	redo.UndoAction = undo.UndoAction;
	m_redoStack.push_back(redo);
}

size_t UndoSystem::UndoCount()
{
	return m_undoStack.size();
}

void UndoSystem::Redo()
{
	RedoCommand redo = m_redoStack.back();
	m_redoStack.pop_back();
	redo.RedoAction();

	UndoCommand undo;
	undo.Action = redo.RedoAction;
	undo.UndoAction = redo.UndoAction;
	m_undoStack.push_back(undo);
}

size_t UndoSystem::RedoCount()
{
	return m_redoStack.size();
}
