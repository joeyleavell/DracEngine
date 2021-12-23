#pragma once
#include "LayerStack.h"
#include "Window.h"
#include "Batch.h"

namespace Ry
{

	class EditorWindow
	{
	public:

		EditorWindow();
		~EditorWindow();

		Window* GetWindow();

		void InitWindow();
		bool WantsClose();
		void Resize(int32 Width, int32 Height);
		void OnEvent(const Event& Ev);

		LayerStack& GetLayerStack();

		virtual void Init();
		virtual void Update(float Delta);
		virtual void Render();

	private:

		static bool bRenderingEngineInitialized;

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
