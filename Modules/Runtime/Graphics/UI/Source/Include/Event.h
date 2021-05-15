#pragma once

#include "Core/Core.h"
#include "UIGen.h"

namespace Ry
{

	constexpr uint32 EVENT_MOUSE        = 0;
	constexpr uint32 EVENT_MOUSE_BUTTON = 1;
	constexpr uint32 EVENT_RESIZE       = 2;

	struct UI_MODULE Event
	{
		uint32 Type;
	};

	struct UI_MODULE MouseEvent : public Event
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		float MouseDeltaX = 0.0f;
		float MouseDeltaY = 0.0f;
	};

	struct UI_MODULE ResizeEvent : public Event
	{
		int32 NewWidth;
		int32 NewHeight;
	};

	struct UI_MODULE MouseButtonEvent : public Event
	{
		int32 ButtonID;
		bool bPressed;
	};

	
}