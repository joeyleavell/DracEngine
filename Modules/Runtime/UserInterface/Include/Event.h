#pragma once

#include "Input.h"
#include "UserInterfaceGen.h"

namespace Ry
{
	
	constexpr uint32 EVENT_MOUSE        = 0;
	constexpr uint32 EVENT_MOUSE_BUTTON = 1;
	constexpr uint32 EVENT_MOUSE_CLICK  = 2;
	constexpr uint32 EVENT_MOUSE_DRAG   = 3;
	constexpr uint32 EVENT_MOUSE_SCROLL = 4;
	constexpr uint32 EVENT_KEY          = 5;
	constexpr uint32 EVENT_CHAR         = 6;
	constexpr uint32 EVENT_PATH_DROP    = 7;

	struct USERINTERFACE_MODULE Event
	{
		uint32 Type;
	};

	struct USERINTERFACE_MODULE MouseScrollEvent : public Event
	{
		double ScrollX = 0.0f;
		double ScrollY = 0.0f;
	};

	struct USERINTERFACE_MODULE MouseEvent : public Event
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		float MouseDeltaX = 0.0f;
		float MouseDeltaY = 0.0f;
	};

	struct USERINTERFACE_MODULE MouseButtonEvent : public Event
	{
		int32 ButtonID = 0;
		bool bPressed = false;
		float MouseX = 0.0f;
		float MouseY = 0.0f;
	};

	struct USERINTERFACE_MODULE MouseClickEvent : public Event
	{
		int32 ButtonID = 0;
		bool bDoubleClick = false;
		float MouseX = 0.0f;
		float MouseY = 0.0f;
	};

	struct USERINTERFACE_MODULE MouseDragEvent : public Event
	{
		int32 ButtonID = 0;
		float MouseX = 0.0f;
		float MouseY = 0.0f;
	};

	struct USERINTERFACE_MODULE KeyEvent : public Event
	{
		KeyAction Action;
		int32 KeyCode;

		bool bAlt;
		bool bShift;
		bool bCtrl;
	};

	struct USERINTERFACE_MODULE CharEvent : public Event
	{
		int32 Codepoint;
	};

	struct USERINTERFACE_MODULE PathDropEvent : public Event
	{
		int32 PathCount;
		const char** Paths;
	};
}
