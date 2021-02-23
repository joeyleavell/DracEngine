#include "GLContext2.h"
#include "glfw3.h"

namespace Ry
{

	GLContext2::GLContext2()
	{
		
	}
	
	bool GLContext2::PreWindowCreation(::GLFWwindow* Window)
	{
		// TODO: do we want these to be configurable?

		return true;
	}

	bool GLContext2::InitContext(::GLFWwindow* Window, SwapChain* SwapChains)
	{
		glfwMakeContextCurrent(Window);

		return true;
	}

	void GLContext2::CleanupContext()
	{
		// Do nothing here - GLFW will cleanup context when destroy window is called
	}
	
}
