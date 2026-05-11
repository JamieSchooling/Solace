#pragma once

#define EVENT_TYPES \
	X(WindowClose) \
	X(WindowResize, int Width; int Height;) \
	X(WindowMove, int PosX; int PosY;) \
	X(WindowFocus) \
	X(WindowFocusLost) \
	X(SceneLoad) \

enum class EventType
{
	None,
#define X(m_name, fields) m_name,
	EVENT_TYPES
#undef X
};

struct Event
{
	EventType Type;
	bool Handled = false;
	
	union
	{
#define X(m_name, fields) struct m_name##Data { fields } m_name##Args;
		EVENT_TYPES
#undef X
	};
};