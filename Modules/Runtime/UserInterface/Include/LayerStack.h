#pragma once

#include "Core/Core.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class Layer;
	struct Event;

	class USERINTERFACE_MODULE LayerStack
	{
	public:

		LayerStack();
		virtual ~LayerStack();

		void PushLayer(Layer* Lay);
		void RemoveLayer(Layer* Lay);

		void Update(float Delta);
		void Render();

		void OnEvent(const Event& Ev);
		void OnResize(int32 NewWidth, int32 NewHeight);		

	private:

		// Acts like a stack
		Ry::ArrayList<Layer*> Layers;
		
	};
	
}
