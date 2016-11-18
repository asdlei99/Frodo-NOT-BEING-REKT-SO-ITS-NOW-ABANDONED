#pragma once

#include "event.h"
#include <math/vec2.h>

class EventWindow : public Event {
protected:
	EventWindow(FD_TYPE type) : Event(type) {}

public:
	EventWindow() : EventWindow(FD_WINDOW_EVENT) {}
};

class EventWindowActionResize : public EventWindow {
private:
	ivec2 size;

public:
	EventWindowActionResize(ivec2 size) : EventWindow(FD_WINDOW_ACTION_RESIZE) { this->size = size; }

	inline ivec2 GetSize() const { return size; }
};

class EventWindowActionMove : public EventWindow {
private:
	ivec2 position;

public:
	EventWindowActionMove(ivec2 position) : EventWindow(FD_WINDOW_ACTION_MOVE) { this->position = position; }

	inline ivec2 GetPosition() const { return position; }
};

class EventWindowState : public EventWindow {
public:
	enum FD_ACTION {
		FD_FOCUS_GAINED,
		FD_FOCUS_LOST,
		FD_MINIMIZED,
		FD_MAXIMIZED
	};

private:
	FD_ACTION action;

public:
	EventWindowState(FD_ACTION action) : EventWindow(action == FD_MAXIMIZED? FD_WINDOW_STATE_MAXIMIZED : action == FD_MINIMIZED ? FD_WINDOW_STATE_MINIMIZED : action == FD_FOCUS_GAINED ? FD_WINDOW_STATE_FOCUS_GAINED : FD_WINDOW_STATE_FOCUS_LOST) { this->action = action; }

	inline FD_ACTION GetAction() const { return action; }
};