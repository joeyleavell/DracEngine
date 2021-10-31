#pragma once

#include "Core/Core.h"
#include "OpenGLGen.h"
#include "Context.h"

namespace Ry
{

	class OPENGL_MODULE GLContext : public Ry::Context
	{
	public:
		GLContext();
		virtual ~GLContext() {  };

		virtual bool PreWindowCreation(::GLFWwindow* Window);

		virtual bool InitContextGL(::GLFWwindow* Window, SwapChain* SwapChains);

		virtual void CleanupContext();
	};
	
}
