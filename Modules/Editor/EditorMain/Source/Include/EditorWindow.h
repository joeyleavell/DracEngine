#pragma once
#include "LayerStack.h"
#include "Window.h"
#include "2D/Batch/Batch.h"

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

		void Update();
		void Render();

		bool WantsClose();

		void Resize(int32 Width, int32 Height);

	private:

		// Layers that get drawn to the 
		LayerStack EdLayers;

		Window* EdWindow;

	};

	
}
