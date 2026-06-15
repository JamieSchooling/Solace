#include "Core/Undo.h"

#include "Core/EditorSystem.h"

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

	EditorSystem::Get().SetSceneDirty();
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

	EditorSystem::Get().SetSceneDirty();
}

size_t UndoSystem::RedoCount()
{
	return m_redoStack.size();
}

ActiveEdit UndoSystem::GetActiveEdit()
{
	return m_activeEdit;
}

void UndoSystem::BeginPropertyEdit(IProperty* property, entt::registry& registry, entt::entity entity)
{
	if (m_activeEdit.Active)
	{
		return;
	}

	m_activeEdit.Active = true;
	m_activeEdit.Property = property;
	m_activeEdit.BeforeValue = property->Get(registry, entity);
}

void UndoSystem::EndPropertyEdit(entt::registry& registry, entt::entity entity, std::function<void(bool)> sideEffect)
{
	if (!m_activeEdit.Active)
	{
		return;
	}

	UndoCommand undo;

	ActiveEdit activeEdit = GetActiveEdit();

	IProperty* property = activeEdit.Property;

	if (property->Type() == PropertyType::Enum)
	{
		auto before = activeEdit.BeforeValue;
		auto after = property->Get(registry, entity);
		undo.Action = [after, property, entity, &registry, sideEffect]()
		{
			EnumInfo info = std::any_cast<EnumInfo>(after);
			property->Set(info.CurrentValue, registry, entity);
			if (sideEffect)
			{
				sideEffect(false);
			}
		};
		undo.UndoAction = [before, property, entity, &registry, sideEffect]()
		{
			EnumInfo info = std::any_cast<EnumInfo>(before);
			property->Set(info.CurrentValue, registry, entity);
			if (sideEffect)
			{
				sideEffect(true);
			}
		};
	}
	else
	{
		auto before = activeEdit.BeforeValue;
		auto after = property->Get(registry, entity);
		undo.Action = [after, property, entity, &registry, sideEffect]()
		{
			property->Set(after, registry, entity);
			if (sideEffect)
			{
				sideEffect(false);
			}
		};
		undo.UndoAction = [before, property, entity, &registry, sideEffect]()
		{
			property->Set(before, registry, entity);
			if (sideEffect)
			{
				sideEffect(true);
			}
		};
	}

	AddUndoCommand(undo);

	m_activeEdit = {};
}

void UndoSystem::BeginValueEdit(std::any beforeValue)
{
	if (m_activeEdit.Active)
	{
		return;
	}

	m_activeEdit.Active = true;
	m_activeEdit.BeforeValue = beforeValue;
}

void UndoSystem::EndValueEdit(std::any afterValue, std::function<void(std::any)> undoAction, std::function<void(std::any)> redoAction)
{
	if (!m_activeEdit.Active)
	{
		return;
	}

	UndoCommand undo;

	ActiveEdit activeEdit = GetActiveEdit();

	auto before = activeEdit.BeforeValue;
	undo.Action = [afterValue, redoAction]()
	{
		if (redoAction)
		{
			redoAction(afterValue);
		}
	};
	undo.UndoAction = [before, undoAction]()
	{
		if (undoAction)
		{
			undoAction(before);
		}
	};

	AddUndoCommand(undo);

	m_activeEdit = {};
}
