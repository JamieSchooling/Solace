#pragma once

#include <functional>
#include <Reflection/Property.h>

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

struct ActiveEdit
{
	bool Active = false;

	IProperty* Property = nullptr;

	std::any BeforeValue;
};

class UndoSystem
{
public:
	static void AddUndoCommand(UndoCommand command);
	static void Undo();
	static size_t UndoCount();
	static void Redo();
	static size_t RedoCount();

	static ActiveEdit GetActiveEdit();

	static void BeginPropertyEdit(IProperty* property, entt::registry& registry, entt::entity entity);
	static void EndPropertyEdit(entt::registry& registry, entt::entity entity, std::function<void(bool)> sideEffect = nullptr);
private:
	static inline std::vector<UndoCommand> m_undoStack;
	static inline std::vector<RedoCommand> m_redoStack;
	static inline ActiveEdit m_activeEdit;
};