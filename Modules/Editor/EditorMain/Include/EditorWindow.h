#pragma once
#include "LayerStack.h"
#include "Window.h"
#include "2D/Batch/Batch.h"
#include "Input.h"
#include "Timer.h"

namespace Ry
{

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

		void OnEvent(const Event& Ev);

	private:

		void OnButtonPressed(int32 Button, bool bPressed);
		void OnKeyPressed(int32 Key, KeyAction Action, int32 Mods);
		void OnKeyChar(uint32 Codepoint);
		void OnScroll(double ScrollX, double ScrollY);
		
		// Layers that get drawn to the 
		LayerStack EdLayers;

		Window* EdWindow;

		double LastScrollX = 0.0f;
		double LastScrollY = 0.0f;

	};

}
