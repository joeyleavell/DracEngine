#include "Window.h"
#include "GLFW/glfw3.h"
#include "Algorithm/Algorithm.h"
#include "Core/Globals.h"

namespace Ry
{
	
	// template <>
	// uint32 Hash<::GLFWwindow*>(const ::GLFWwindow* Object)
	// {
	// 	return reinterpret_cast<uint32>(Object);
	// }

	template <>
	uint32 Hash<::GLFWwindow*>(::GLFWwindow* const& Object)
	{
		return reinterpret_cast<uint64>(Object);
	}
	
	Ry::Map<::GLFWwindow*, Window*> Window::Windows;

	Window::Window(){}

	bool Window::CreateWindow(int32 Width, int32 Height)
	{
		return CreateWindow("", Width, Height);
	}

	bool Window::CreateWindow(Ry::String Title, int32 Width, int32 Height)
	{
		return CreateWindow(Title, Width, Height, -1);
	}

	bool Window::CreateWindow(Ry::String Title, int32 Width, int32 Height, int32 FSMonitor)
	{
		this->FullscreenMonitor = FullscreenMonitor;

		if (!glfwInit())
		{
			Ry::Log->LogError("GLFW initialization failed!");
			
			return false;
		}

		{
			glfwSetErrorCallback(&ErrorCallback);

			// Setup hints for rendering API

			glfwWindowHint(GLFW_RED_BITS, 8);
			glfwWindowHint(GLFW_GREEN_BITS, 8);
			glfwWindowHint(GLFW_BLUE_BITS, 8);
			glfwWindowHint(GLFW_ALPHA_BITS, 8);
			glfwWindowHint(GLFW_DEPTH_BITS, 32);
			glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

			// TODO: do window hints separately
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		}


		if(FSMonitor >= 0)
		{
			GLFWmonitor* Monitor = GetMonitorByIndex(FullscreenMonitor);

			if (Monitor)
			{
				const GLFWvidmode* Mode = glfwGetVideoMode(Monitor);
				WindowResource = glfwCreateWindow(Width, Height, *Title, Monitor, nullptr);
			}
			else
			{
				Ry::Log->LogError("Failed to find fullscreen monitor with index " + Ry::to_string(FullscreenMonitor));
				return false;
			}
		}
		else
		{
			WindowResource = glfwCreateWindow(Width, Height, *Title, nullptr, nullptr);
		}

		if (!WindowResource)
		{

			Ry::Log->LogError("GLFW window creation failed");
			return false;
		}
		else
		{
			// Setup input callbacks
			glfwSetKeyCallback(WindowResource, &KeyCallback);
			glfwSetCharCallback(WindowResource, &CharacterEntryCallback);
			glfwSetMouseButtonCallback(WindowResource, &MouseButtonCallback);
			glfwSetScrollCallback(WindowResource, &ScrollCallback);

			// Setup window callbacks
			glfwSetWindowSizeCallback(WindowResource, &WindowSizeCallback);
		}

		// Insert window into static map
		Windows.insert(WindowResource, this);

		return true;
	}

	void Window::SetVSyncEnabled(bool bEnabled)
	{
		/**
		 * TODO: this implementation is currently OpenGL specific
		 */

		if(bEnabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
	}

	bool Window::IsFullscreen() const
	{
		return glfwGetWindowMonitor(WindowResource) != nullptr;
	}

	void Window::SetFullscreen(bool fullscreen)
	{
		this->SetFullscreen(fullscreen, 0);
	}

	void Window::SetFullscreen(bool Fullscreen, int32 MonitorIndex)
	{
		if (IsFullscreen() == Fullscreen)
			return;

		if (Fullscreen)
		{

			// Store previous window position and size
			glfwGetWindowPos(WindowResource, &WindowPosBuffer[0], &WindowPosBuffer[1]);
			glfwGetWindowSize(WindowResource, &WindowSizeBuffer[0], &WindowSizeBuffer[1]);

			// Retrieve the resolution of the monitor that will host the fullscreen
			GLFWmonitor* Monitor = GetMonitorByIndex(MonitorIndex);
			const GLFWvidmode* mode = glfwGetVideoMode(Monitor);
			WindowSizeCallback(WindowResource, mode->width, mode->height); // Notify of new viewport size previous to full screen

			// switch to full screen
			glfwSetWindowMonitor(WindowResource, Monitor, 0, 0, mode->width, mode->height, 0);

			FullscreenMonitor = MonitorIndex;
		}
		else
		{
			// restore last window size and position
			glfwSetWindowMonitor(WindowResource, nullptr, WindowPosBuffer[0], WindowPosBuffer[1], WindowSizeBuffer[0], WindowSizeBuffer[1], 0);

			FullscreenMonitor = -1;
		}

	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(WindowResource);
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(WindowResource);
	}

	void Window::Update()
	{
		// Poll window events
		glfwPollEvents();
	}

	void Window::InitContext()
	{
		glfwMakeContextCurrent(WindowResource);
	}

	void Window::Destroy()
	{
		glfwDestroyWindow(WindowResource);
		glfwTerminate();
	}

	float Window::GetCursorX()
	{
		double MouseX, MouseY;
		glfwGetCursorPos(WindowResource, &MouseX, &MouseY);

		return MouseX;
	}

	float Window::GetCursorY()
	{
		double MouseX, MouseY;
		glfwGetCursorPos(WindowResource, &MouseX, &MouseY);

		return MouseY;
	}

	void Window::GetCursorPos(double& X, double& Y)
	{
		glfwGetCursorPos(WindowResource, &X, &Y);
	}

	void Window::AddWindowResizedDelegate(const Ry::Delegate<void, int32, int32>& Delegate)
	{
		WindowResizedDelegates.Add(Delegate);
	}

	void Window::AddKeyPressDelegate(const Ry::Delegate<void, int32, bool>& Delegate)
	{
		KeyCallbacks.Add(Delegate);
	}

	void Window::AddKeyCharDelegate(const Ry::Delegate<void, uint32>& Delegate)
	{
		CharacterCallbacks.Add(Delegate);
	}

	void Window::AddMouseButtonDelegate(const Ry::Delegate<void, int32, bool>& Delegate)
	{
		MouseButtonCallbacks.Add(Delegate);
	}

	void Window::AddScrollDelegate(const Ry::Delegate<void, double, double>& Delegate)
	{
		ScrollCallbacks.Add(Delegate);
	}


	Window* Window::FindWindow(::GLFWwindow* Window)
	{
		Ry::Window** Result = Windows.get(Window);

		if (!Result)
		{
			return nullptr;
		}
		else
		{
			return *Result;
		}
	}

	void Window::ErrorCallback(int Error, const char* Desc)
	{
		Ry::Log->LogError("GLFW error: " + Ry::to_string(Error) + ": " + Ry::String(Desc));
	}

	void Window::KeyCallback(::GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
	{
		Ry::Window* AssociatedWindow = FindWindow(Window);

		if (AssociatedWindow)
		{
			for (const Delegate<void, int32, bool>& Callback : AssociatedWindow->KeyCallbacks)
			{
				if (Action == GLFW_PRESS)
				{
					Callback.Execute(Key, true);
				}
				else if (Action == GLFW_RELEASE)
				{
					Callback.Execute(Key, false);
				}
			}
		}
	}

	void Window::MouseButtonCallback(::GLFWwindow* Window, int Button, int Action, int Mods)
	{
		Ry::Window* AssociatedWindow = FindWindow(Window);

		if (AssociatedWindow)
		{
			for (const Delegate<void, int32, bool>& Callback : AssociatedWindow->MouseButtonCallbacks)
			{
				if (Action == GLFW_PRESS)
				{
					Callback.Execute(Button, true);
				}
				else if (Action == GLFW_RELEASE)
				{
					Callback.Execute(Button, false);
				}
			}
			// for (const Delegate<void, int32, bool>& Callback : AssociatedWindow->MouseButtonCallbacks)
			// {
			//
			// }
		}
	}

	void Window::ScrollCallback(::GLFWwindow* Window, double XOff, double YOff)
	{
		Ry::Window* AssociatedWindow = FindWindow(Window);

		if (AssociatedWindow)
		{
			for (const Delegate<void, double, double>& Callback : AssociatedWindow->ScrollCallbacks)
			{
				Callback.Execute(XOff, YOff);
			}
		}
	}

	void Window::CharacterEntryCallback(::GLFWwindow* Window, unsigned int Codepoint)
	{
		Ry::Window* AssociatedWindow = FindWindow(Window);

		if (AssociatedWindow)
		{
			for (const Delegate<void, uint32>& Callback : AssociatedWindow->CharacterCallbacks)
			{
				Callback.Execute(Codepoint);
			}
		}
	}

	void Window::WindowSizeCallback(::GLFWwindow* Window, int NewWidth, int NewHeight)
	{
		Ry::Window* AssociatedWindow = FindWindow(Window);

		if (AssociatedWindow)
		{
			for (const Delegate<void, int32, int32>& Callback : AssociatedWindow->WindowResizedDelegates)
			{
				Callback.Execute(NewWidth, NewHeight);
			}
		}
	}

	GLFWmonitor* Window::GetMonitorByIndex(int32 index)
	{
		// Get the correct monitor
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		GLFWmonitor* monitor = nullptr;

		if (index >= count)
		{
			Ry::Log->LogError("Full screen monitor " + Ry::to_string(index) + " out of bounds.");
			return monitors[0];
		}
		else
		{
			return monitors[index];
		}
	}

	int32 Window::GetWindowWidth() const
	{
		if (IsFullscreen())
		{
			return glfwGetVideoMode(GetMonitorByIndex(FullscreenMonitor))->width;
		}
		else
		{
			int32 width, height;
			glfwGetWindowSize(WindowResource, &width, &height);

			return width;
		}
	}

	int32 Window::GetWindowHeight() const
	{
		if (IsFullscreen())
		{
			return glfwGetVideoMode(GetMonitorByIndex(FullscreenMonitor))->height;
		}
		else
		{
			int32 width, height;
			glfwGetWindowSize(WindowResource, &width, &height);

			return height;
		}
	}
	
}
