#pragma once

#define EVENT_TYPES \
	X(WindowClose) \
	X(WindowResize, int width; int height;) \
	X(WindowMove, int posX; int posY;) \
	X(WindowFocus) \
	X(WindowFocusLost) \
	X(SceneLoad) \

enum class EventType
{
	None,
#define X(name, fields) name,
	EVENT_TYPES
#undef X
};

struct Event
{
	EventType type;
	bool handled = false;
	
	union
	{
#define X(name, fields) struct name##Data { fields } name##Args;
		EVENT_TYPES
#undef X
	};
};