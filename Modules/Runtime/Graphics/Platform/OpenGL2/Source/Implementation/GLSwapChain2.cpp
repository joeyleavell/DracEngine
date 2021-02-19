#include "GLSwapChain2.h"
#include "glew.h"
#include "glfw3.h"
#include "Core/Globals.h"

namespace Ry
{
	void GLSwapChain2::BeginFrame(::GLFWwindow* Window, bool bWindowDirty)
	{
		if(bWindowDirty)
		{
			glfwGetFramebufferSize(Window, &SwapChainWidth, &SwapChainHeight);
		}

		// Make the OpenGL context current
		//glfwMakeContextCurrent(Window);
	}

	void GLSwapChain2::EndFrame(::GLFWwindow* Window)
	{
		glfwSwapBuffers(Window);
	}

	bool GLSwapChain2::CreateSwapChain(::GLFWwindow* ParentWindow)
	{
		glfwGetFramebufferSize(ParentWindow, &SwapChainWidth, &SwapChainHeight);

		glfwMakeContextCurrent(ParentWindow);

		glfwSwapInterval(0);

		// todo: Make this work with multiple contexts
		if (glewInit() != GLEW_OK)
		{
			Ry::Log->LogError("Failed to init GLEW");
			return false;
		}
		else
		{
			Ry::Log->Log("GLEW loaded");

			int OpenGLMajor;
			int OpenGLMinor;
			int32 FBOSamples;
			glGetIntegerv(GL_MAJOR_VERSION, &OpenGLMajor);
			glGetIntegerv(GL_MINOR_VERSION, &OpenGLMinor);
			glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &FBOSamples);

			// Todo: print other useful info here?
			Ry::Log->Log("OpenGL version: " + Ry::to_string(OpenGLMajor) + "." + Ry::to_string(OpenGLMinor));
			Ry::Log->Log("\tMax FBO samples: " + Ry::to_string(FBOSamples));
		}

		return true;
	}

	void GLSwapChain2::DeleteSwapChain()
	{
		
	}

	int32 GLSwapChain2::GetSwapChainWidth() const
	{
		return SwapChainWidth;
	}

	int32 GLSwapChain2::GetSwapChainHeight() const
	{
		return SwapChainHeight;
	}

	RenderPass2* GLSwapChain2::GetDefaultRenderPass()
	{
		return nullptr;
	}

}
