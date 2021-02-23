#pragma once

#include "Core/Core.h"
#include "OpenGL2Gen.h"
#include "Context.h"

namespace Ry
{

	class OPENGL2_MODULE GLContext2 : public Ry::Context
	{
	public:
		GLContext2();
		virtual ~GLContext2() {  };

		virtual bool PreWindowCreation(::GLFWwindow* Window);

		virtual bool InitContext(::GLFWwindow* Window, SwapChain* SwapChains);

		virtual void CleanupContext();
	};
	
}
