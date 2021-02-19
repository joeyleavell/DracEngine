#pragma once

#include "RenderingGen.h"

struct GLFWwindow;

namespace Ry
{

	class SwapChain;

	class RENDERING_MODULE Context
	{
	public:
		Context();
		virtual ~Context();

		virtual bool PreWindowCreation(::GLFWwindow* Window) = 0;
		virtual bool InitContext() = 0;
		virtual void CleanupContext() = 0;

	};
	
}