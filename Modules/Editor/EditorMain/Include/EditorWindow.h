#pragma once
#include "LayerStack.h"
#include "Window.h"
#include "2D/Batch/Batch.h"
#include "Input.h"
#include "Timer.h"

namespace Ry
{

	struct MouseEventInfo
	{
		// Single click delay
		Timer ClickTimer {0.5};
		
		int32 Button = 0;

		// Whether the current button is pressed
		bool bIsPressed = false;

		// Dragging is mutually exclusive with clicking
		// Dragging begins after the mouse cursor has moved a certain radius away from the initial click point
		bool bDrag = false;

		int32 ClickCount = 0;
		
		// When a press is initiated, this is recorded since
		// it controls if the event was a click or drag
		int32 StartX = 0;
		int32 StartY = 0;
		
	};

	class EditorWindow
	{
	public:

		EditorWindow();
		~EditorWindow();

		int32 GetViewportWidth();
		int32 GetViewportHeight();

		void InitWindow();
		void Init();

		void Update(float Delta);
		void Render();

		bool WantsClose();

		void Resize(int32 Width, int32 Height);

	private:

		void OnButtonPressed(int32 Button, bool bPressed);
		void OnKeyPressed(int32 Key, KeyAction Action, int32 Mods);
		void OnKeyChar(uint32 Codepoint);
		void OnScroll(double ScrollX, double ScrollY);

		void FireClick(int32 Button, double XPos, double YPos);
		void FireDoubleClick(int32 Button, double XPos, double YPos);
		void FireDragEvent(int32 Button, float XPos, float YPos);
		void FireButtonEvent(int32 Button, float XPos, float YPos, bool bPressed);
		void FireKeyEvent(int32 KeyCode, KeyAction Action, int32 Mods);
		void FireCharEvent(int32 Codepoint);

		void FireScrollEvent(float ScrollX, float ScrollY);

		// Layers that get drawn to the 
		LayerStack EdLayers;

		Window* EdWindow;

		float HigherOrderClickInterval = 0.5f; // Amount of time allowed between clicks in higher-order click events (double press, triple press, etc.)
		float SingleClickInterval      = 0.5f; // Amount of time allowed between clicks in higher-order click events (double press, triple press)
		float ClickDistThreshold       = 10.0f; // Pixel deviation allowed before drag event is triggered

		MouseEventInfo ButtonsInfo[MAX_BUTTONS];

		double LastScrollX = 0.0f;
		double LastScrollY = 0.0f;

	};

}
