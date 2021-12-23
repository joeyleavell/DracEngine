#pragma once

#include "Event.h"
#include "Interface/SwapChain.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE Layer
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
