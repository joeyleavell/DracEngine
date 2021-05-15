#pragma once

#include "Event.h"
#include "UIGen.h"
#include "SwapChain.h"

namespace Ry
{

	class UI_MODULE Layer
	{
	public:

		Layer(SwapChain* Parent);

		virtual void Update(float Delta) {};
		virtual void Render() {};
		virtual bool OnEvent(const Event& Ev) { return false; };

		virtual void OnResize(int32 Width, int32 Height){};

	protected:

		SwapChain* ParentSC;
		
	};
	
}
