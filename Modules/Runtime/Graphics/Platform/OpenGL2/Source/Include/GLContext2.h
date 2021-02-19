#pragma once

#include "Core/Core.h"
#include "OpenGL2Gen.h"
#include "Context.h"

namespace Ry
{

	class OPENGL2_MODULE GLContext : public Ry::Context
	{
	public:
		GLContext();
		virtual ~GLContext() {  };

		virtual bool PreWindowCreation(::GLFWwindow* Window);

		virtual bool InitContext(::GLFWwindow* Window, SwapChain* SwapChains);

		virtual void CleanupContext();
	};
	
}
