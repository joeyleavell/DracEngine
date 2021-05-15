#pragma once

#include "Event.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE Layer
	{
	public:

		void Update(float Delta) {};
		void Render() {};
		bool OnEvent(const Event& Ev) { return false; };
		
	};
	
}