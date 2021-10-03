#include "Window.h"
#include "Algorithm/Algorithm.h"
#include "Core/Globals.h"
#include "RenderingEngine.h"
#include "glfw3.h"
#include <set>
#include "GLSwapChain.h"
#include "VulkanContext.h"
#include "VulkanSwapChain.h"
#include "SwapChain.h"
#include "Input.h"
#include "Event.h"
#include "Keys.h"
#include "Timer.h"

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
	
	static Ry::Map<::GLFWwindow*, Window*> Windows;

	bool InitWindowing()
	{
		// Setup the GLFW error callback early on
		glfwSetErrorCallback(&WindowingErrorCallback);

		if (!glfwInit())
		{
			Ry::Log->LogError("GLFW initialization failed!");
			return false;
		}

		return true;
	}

	void WindowingErrorCallback(int Error, const char* Desc)
	{
		Ry::Log->LogError("GLFW: " + Ry::to_string(Error) + ": " + Ry::String(Desc));
	}

	MouseEventInfo::MouseEventInfo()
	{
		ClickTimer = new Timer{ 0.5f };
	}

	MouseEventInfo::~MouseEventInfo()
	{
		delete ClickTimer;
	}

	Window::Window(Ry::RenderingPlatform Plat)
	{
		this->Platform = Plat;

		ButtonsInfo = new MouseEventInfo[MAX_BUTTONS];
	}

	Window::~Window()
	{
		delete[] ButtonsInfo;
	}

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
		this->FullscreenMonitor = FSMonitor;

		SetupWindowHints();

		// Window creation logic depending on if we want fullscreen
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

		// Check that the window was created successfully
		if (!WindowResource)
		{
			Ry::Log->LogError("GLFW window creation failed");
			return false;
		}

		// Create the rendering api specific swap chain
		// Todo: investigate creating multiple contexts per window/swapchain, and multiple windows per context/swapchain
		CreateSwapChain();

		// Setup callbacks
		glfwSetKeyCallback(WindowResource, &KeyCallback);
		glfwSetCharCallback(WindowResource, &CharacterEntryCallback);
		glfwSetMouseButtonCallback(WindowResource, &MouseButtonCallback);
		glfwSetScrollCallback(WindowResource, &ScrollCallback);
		glfwSetWindowSizeCallback(WindowResource, &WindowSizeCallback);
		glfwSetFramebufferSizeCallback(WindowResource, &FrameBufferSizeCallback);

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

	void Window::BeginFrame()
	{
		SwapChain->BeginFrame(WindowResource, bFramebufferResized);
	}

	void Window::EndFrame()
	{
		SwapChain->EndFrame(WindowResource);

		// Invoke window size callbacks since swap chain delayed recreation
		if (bFramebufferResized)
		{
			bFramebufferResized = false;

			int Width, Height;
			glfwGetFramebufferSize(WindowResource, &Width, &Height);
			
			for (const Delegate<void, int32, int32>& Callback : WindowResizedDelegates)
			{
				Callback.Execute(Width, Height);
			}
		}
		
	}

	void Window::FireClick(int32 Button, double XPos, double YPos)
	{
		// Fire double press event
		MouseClickEvent ClickEvent{};
		ClickEvent.Type = EVENT_MOUSE_CLICK;
		ClickEvent.ButtonID = Button;
		ClickEvent.bDoubleClick = false;
		ClickEvent.MouseX = XPos;
		ClickEvent.MouseY = YPos;

		OnEvent.Broadcast(ClickEvent);
	}

	void Window::FireDoubleClick(int32 Button, double XPos, double YPos)
	{
		// Fire double press event
		MouseClickEvent DoubleClickEvent{};
		DoubleClickEvent.Type = EVENT_MOUSE_CLICK;
		DoubleClickEvent.ButtonID = Button;
		DoubleClickEvent.bDoubleClick = true;
		DoubleClickEvent.MouseX = XPos;
		DoubleClickEvent.MouseY = YPos;

		OnEvent.Broadcast(DoubleClickEvent);
	}

	void Window::FireDragEvent(int32 Button, float XPos, float YPos)
	{
		MouseDragEvent DragEvent{};
		DragEvent.Type = EVENT_MOUSE_DRAG;
		DragEvent.ButtonID = Button;
		DragEvent.MouseX = XPos;
		DragEvent.MouseY = YPos;

		OnEvent.Broadcast(DragEvent);
	}

	void Window::FireButtonEvent(int32 Button, float XPos, float YPos, bool bPressed)
	{
		MouseButtonEvent ButtonEvent{};
		ButtonEvent.Type = EVENT_MOUSE_BUTTON;
		ButtonEvent.ButtonID = Button;
		ButtonEvent.MouseX = XPos;
		ButtonEvent.MouseY = YPos;
		ButtonEvent.bPressed = bPressed;

		OnEvent.Broadcast(ButtonEvent);
	}

	void Window::FireKeyEvent(int32 KeyCode, KeyAction Action, int32 Mods)
	{
		Ry::KeyEvent Ev;
		Ev.Type = EVENT_KEY;
		Ev.Action = Action;
		Ev.KeyCode = KeyCode;
		Ev.bCtrl = (Mods & MOD_CONTROL) == MOD_CONTROL;
		Ev.bShift = (Mods & MOD_SHIFT) == MOD_SHIFT;
		Ev.bAlt = (Mods & MOD_ALT) == MOD_ALT;

		OnEvent.Broadcast(Ev);
	}

	void Window::FireCharEvent(int32 Codepoint)
	{
		CharEvent Ev;
		Ev.Type = EVENT_CHAR;
		Ev.Codepoint = Codepoint;

		OnEvent.Broadcast(Ev);
	}

	void Window::FireScrollEvent(float ScrollX, float ScrollY)
	{
		MouseScrollEvent ScrollEvent{};
		ScrollEvent.Type = EVENT_MOUSE_SCROLL;
		ScrollEvent.ScrollX = ScrollX;
		ScrollEvent.ScrollY = ScrollY;

		OnEvent.Broadcast(ScrollEvent);
	}


	void Window::Update()
	{
		// Poll window events
		glfwPollEvents();

		double MouseX, MouseY;
		GetCursorPos(MouseX, MouseY);
		MouseY = GetWindowHeight() - MouseY - 1;

		// Send mouse pos event
		{

			MouseEvent Ev;
			Ev.Type = EVENT_MOUSE;
			Ev.MouseX = (float)MouseX;
			Ev.MouseY = (float)MouseY;
			Ev.MouseDeltaX = 0.0f;
			Ev.MouseDeltaY = 0.0f;

			OnEvent.Broadcast(Ev);
		}

		// Handle drag events
		for (int32 Index = 0; Index < MAX_BUTTONS; Index++)
		{
			MouseEventInfo& Info = ButtonsInfo[Index];

			// Check if click event is still applicable
			if (Info.bIsPressed)
			{
				if (!Info.bDrag)
				{
					// Distance factor
					double DX = MouseX - Info.StartX;
					double DY = MouseY - Info.StartY;
					double Dist = sqrt(DX * DX + DY * DY);

					if (Dist >= ClickDistThreshold)
					{
						Info.bDrag = true;
					}
				}

				// Fire drag event
				if (Info.bDrag)
				{
					FireDragEvent(Index, MouseX, MouseY);
				}
			}
			else
			{
				// No longer pressed, means stop firing drag event
				Info.bDrag = false;
			}

		}
	}

	void Window::Destroy()
	{
		SwapChain->DeleteSwapChain();
				
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

	void Window::AddKeyPressDelegate(const Ry::Delegate<void, int32, KeyAction, int32>& Delegate)
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

	void Window::KeyCallback(::GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
	{
		Ry::Window* AssociatedWindow = FindWindow(Window);

		if (AssociatedWindow)
		{
			KeyAction EngineAction;
			if (Action == GLFW_PRESS)
				EngineAction = KeyAction::PRESS;
			if (Action == GLFW_RELEASE)
				EngineAction = KeyAction::RELEASE;
			if (Action == GLFW_REPEAT)
				EngineAction = KeyAction::REPEAT;
			AssociatedWindow->FireKeyEvent(Key, EngineAction, Mods);

			for (const Delegate<void, int32, KeyAction, int32>& Callback : AssociatedWindow->KeyCallbacks)
			{
				if (Action == GLFW_PRESS)
				{
					Callback.Execute(Key, KeyAction::PRESS, Mods);
				}
				else if (Action == GLFW_RELEASE)
				{
					Callback.Execute(Key, Ry::KeyAction::RELEASE, Mods);
				}
				else if(Action == GLFW_REPEAT)
				{
					Callback.Execute(Key, KeyAction::REPEAT, Mods);
				}

			}
		}
	}

	void Window::MouseButtonCallback(::GLFWwindow* Window, int Button, int Action, int Mods)
	{
		Ry::Window* AssociatedWindow = FindWindow(Window);

		if (AssociatedWindow)
		{
			MouseEventInfo& EventInfo = AssociatedWindow->ButtonsInfo[Button];

			EventInfo.bIsPressed = (Action == GLFW_PRESS);

			double CurX, CurY;
			AssociatedWindow->GetCursorPos(CurX, CurY);
			CurY = AssociatedWindow->GetWindowHeight() - CurY - 1;

			// TODO: Fire raw mouse pressed/released event
			AssociatedWindow->FireButtonEvent(Button, CurX, CurY, EventInfo.bIsPressed);

			// Calc distance
			float Dx = CurX - EventInfo.StartX;
			float Dy = CurY - EventInfo.StartY;
			float Dist = std::sqrt(Dx * Dx + Dy * Dy);

			// Upon the first press, double click events become eligible
			if (EventInfo.bIsPressed)
			{
				// Record start pos
				EventInfo.StartX = (int32)CurX;
				EventInfo.StartY = (int32)CurY;

				// Reset click count if enough time has passed, or passed distance threshold
				if (Dist >= AssociatedWindow->ClickDistThreshold || EventInfo.ClickTimer->is_ready())
				{
					EventInfo.ClickCount = 0;
				}

				EventInfo.ClickTimer->restart();
			}
			else
			{
				// Distance and time check
				if (Dist <= AssociatedWindow->ClickDistThreshold && !EventInfo.ClickTimer->is_ready())
				{
					EventInfo.ClickCount++;

					if (EventInfo.ClickCount == 1)
					{
						// Fire mouse click, 
						AssociatedWindow->FireClick(Button, CurX, CurY);
					}
					else if (EventInfo.ClickCount == 2)
					{
						AssociatedWindow->FireDoubleClick(Button, CurX, CurY);
					}
					else if (EventInfo.ClickCount == 3) // Triple click
					{
						//FireDoubleClick(Button, CurX, CurY);
						EventInfo.ClickCount = 0;
					}
					else
					{
						// Higher order clicks not supported, reset clicks
						EventInfo.ClickCount = 0;
					}

				}

			}
			
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
			AssociatedWindow->FireScrollEvent(XOff, YOff);

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
			AssociatedWindow->FireCharEvent(Codepoint);

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

		}
	}

	void Window::FrameBufferSizeCallback(::GLFWwindow* Window, int NewWidth, int NewHeight)
	{
		Ry::Window* AssociatedWindow = FindWindow(Window);

		if (AssociatedWindow)
		{
			AssociatedWindow->bFramebufferResized = true;
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

	Ry::SwapChain* Window::GetSwapChain()
	{
		return SwapChain;
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
			glfwGetFramebufferSize(WindowResource, &width, &height);

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
			glfwGetFramebufferSize(WindowResource, &width, &height);

			return height;
		}
	}

	void Window::GetWindowSize(int32& Width, int32& Height)
	{
		glfwGetFramebufferSize(WindowResource, &Width, &Height);
	}

	void Window::CreateSwapChain()
	{
		if(Platform == RenderingPlatform::OpenGL)
		{
			// todo: create gl swap chain
			SwapChain = new Ry::GLSwapChain2;
		}
		else if(Platform == RenderingPlatform::Vulkan)
		{
			SwapChain = new Ry::VulkanSwapChain;
		}

		SwapChain->CreateSwapChain(WindowResource);
	}

	void Window::SetupWindowHints()
	{
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		if (Platform == RenderingPlatform::OpenGL)
		{
			// todo: create gl swap chain
			glfwWindowHint(GLFW_RED_BITS, 8);
			glfwWindowHint(GLFW_GREEN_BITS, 8);
			glfwWindowHint(GLFW_BLUE_BITS, 8);
			glfwWindowHint(GLFW_ALPHA_BITS, 8);
			glfwWindowHint(GLFW_DEPTH_BITS, 32);
			glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		}
		else if (Platform == RenderingPlatform::Vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
	}
}
