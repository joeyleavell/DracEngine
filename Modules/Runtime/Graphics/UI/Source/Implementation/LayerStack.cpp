#include "LayerStack.h"
#include "Event.h"
#include "Layer.h"

namespace Ry
{
	LayerStack::LayerStack()
	{
		
	}

	LayerStack::~LayerStack()
	{
		
	}

	void LayerStack::PushLayer(Layer* Lay)
	{
		Layers.Add(Lay);
	}

	void LayerStack::RemoveLayer(Layer* Lay)
	{
		Layers.Remove(Lay);
	}

	void LayerStack::Update(float Delta)
	{
		// Update from top to bottom
		for(int32 LayerIndex = Layers.GetSize() - 1; LayerIndex >= 0; LayerIndex--)
		{
			Layer* Lay = Layers[LayerIndex];

			Lay->Update(Delta);
		}
		
	}

	void LayerStack::Render()
	{
		// Render from bottom to top
		for (int32 LayerIndex = 0; LayerIndex < Layers.GetSize(); LayerIndex++)
		{
			Layer* Lay = Layers[LayerIndex];

			Lay->Render();
		}
	}

	void LayerStack::OnEvent(const Event& Ev)
	{
		// Pass from top to bottom until handled
		for (int32 LayerIndex = Layers.GetSize() - 1; LayerIndex >= 0; LayerIndex--)
		{
			Layer* Lay = Layers[LayerIndex];

			if(Lay->OnEvent(Ev))
			{
				return;
			}
		}

	}

	void LayerStack::OnResize(int32 NewWidth, int32 NewHeight)
	{
		for (int32 LayerIndex = Layers.GetSize() - 1; LayerIndex >= 0; LayerIndex--)
		{
			Layer* Lay = Layers[LayerIndex];
			Lay->OnResize(NewWidth, NewHeight);
		}
	}
}
