#pragma once

#include "Core/Core.h"
#include "UIGen.h"

namespace Ry
{

	constexpr uint32 EVENT_MOUSE        = 0;
	constexpr uint32 EVENT_MOUSE_BUTTON = 1;
	constexpr uint32 EVENT_MOUSE_CLICK  = 2;
	constexpr uint32 EVENT_MOUSE_DRAG   = 3;
	constexpr uint32 EVENT_MOUSE_SCROLL = 4;

	struct UI_MODULE Event
	{
		uint32 Type;
	};

	struct UI_MODULE MouseScrollEvent : public Event
	{
		double ScrollX = 0.0f;
		double ScrollY = 0.0f;
	};

	struct UI_MODULE MouseEvent : public Event
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		float MouseDeltaX = 0.0f;
		float MouseDeltaY = 0.0f;
	};

	struct UI_MODULE MouseButtonEvent : public Event
	{
		int32 ButtonID = 0;
		bool bPressed = false;
		float MouseX = 0.0f;
		float MouseY = 0.0f;
	};

	struct UI_MODULE MouseClickEvent : public Event
	{
		int32 ButtonID = 0;
		bool bDoubleClick = false;
	};

	struct UI_MODULE MouseDragEvent : public Event
	{
		int32 ButtonID = 0;
		float MouseX = 0.0f;
		float MouseY = 0.0f;
	};

	
}