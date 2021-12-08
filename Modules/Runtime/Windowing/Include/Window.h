#pragma once

#include "Core/String.h"
#include "Data/ArrayList.h"
#include "Data/Map.h"
#include "Algorithm/Algorithm.h"
#include "Core/Core.h"
#include "WindowingGen.h"
//#include "Core/Delegate.h"

struct GLFWwindow;
struct GLFWmonitor;
struct GLFWcursor;

namespace Ry
{
	
	enum class KeyAction;
	
	class CommandBuffer;
	class SwapChain;
	class Context;
	class Timer;
	struct Event;

	bool WINDOWING_MODULE InitWindowing();
	void WindowingErrorCallback(int Error, const char* Description);


	struct MouseEventInfo
	{
		// Single click delay
		Timer* ClickTimer;

		int32 Button = 0;

		// Whether the current button is pressed
		bool bIsPressed = false;

		// Dragging is mutually exclusive with clicking
		// Dragging begins after the mouse cursor has moved a certain radius away from the initial click point
		bool bDrag = false;

		int32 ClickCount = 0;

		// When a press is initiated, this is recorded since
		// it controls if the event was a click or drag
		int32 StartX = 0;
		int32 StartY = 0;

		MouseEventInfo();
		virtual ~MouseEventInfo();

	};

	struct WindowManipData
	{
		bool bIsResizingX{ false };
		bool bIsResizingTop{ false };
		bool bIsResizingBottom{ false };
		bool bIsResizingLeft{ false };
		bool bIsResizingRight { false };

		bool bIsDragging{false};

		int32 InitialWindowXSize{};
		int32 InitialWindowYSize{};
		int32 InitialWindowXPos{};
		int32 InitialWindowYPos{};
		double InitialX{};
		double InitialY{};

		bool IsManipulating()
		{
			return bIsDragging || bIsResizingTop || bIsResizingBottom || bIsResizingLeft || bIsResizingRight;
		}

		void StopManip()
		{
			bIsDragging = false;
			bIsResizingTop = false;
			bIsResizingBottom = false;
			bIsResizingLeft = false;
			bIsResizingRight = false;

		}
	};

	class WINDOWING_MODULE Window
	{
	public:

		MulticastDelegate<const Event&> OnEvent;

		// Todo: support multiple contexts per window in the future?
		Window(Ry::RenderingPlatform Plat);
		virtual ~Window();

		bool CreateWindow(int32 Width, int32 Height);
		bool CreateWindow(Ry::String Title, int32 Width, int32 Height);
		bool CreateWindow(Ry::String Title, int32 Width, int32 Height, int32 FSMonitor);

		/* TODO: this will need extra considerations with Vulkan */
		void SetVSyncEnabled(bool bEnabled);

		void Destroy();

		Ry::SwapChain* GetSwapChain();

		int32 FindWindowWidth() const;
		int32 FindWindowHeight() const;

		int32 GetFramebufferWidth() const;
		int32 GetFramebufferHeight() const;
		void GetFramebufferSize(int32& Width, int32& Height);

		/* TODO: this will need extra considerations with Vulkan */
		bool IsFullscreen() const;
		void SetFullscreen(bool Fullscreen);
		void SetFullscreen(bool Fullscreen, int32 MonitorIndex);

		float GetCursorX();
		float GetCursorY();
		void GetCursorPos(double& X, double& Y);

		bool ShouldClose();

		void BeginFrame();
		void EndFrame();

		void Update();

		void AddWindowResizedDelegate(const Ry::Delegate<void, int32, int32>& Delegate);
		void AddKeyPressDelegate(const Ry::Delegate<void, int32, KeyAction, int32>& Delegate);
		void AddKeyCharDelegate(const Ry::Delegate<void, uint32>& Delegate);
		void AddMouseButtonDelegate(const Ry::Delegate<void, int32, bool>& Delegate);
		void AddScrollDelegate(const Ry::Delegate<void, double, double>& Delegate);

	private:

		double WindowPosX{};
		double WindowPosY{};

		WindowManipData ManipData;
		MouseEventInfo* ButtonsInfo;

		float ClickDistThreshold = 10.0f; // Pixel deviation allowed before drag event is triggered

		void FireScrollEvent(float ScrollX, float ScrollY);
		void FireClick(int32 Button, double XPos, double YPos);
		void FireDoubleClick(int32 Button, double XPos, double YPos);
		void FireDragEvent(int32 Button, float XPos, float YPos);
		void FireButtonEvent(int32 Button, float XPos, float YPos, bool bPressed);
		void FireKeyEvent(int32 KeyCode, KeyAction Action, int32 Mods);
		void FireCharEvent(int32 Codepoint);
		void FirePathDropEvent(int32 PathCount, const char* Paths[]);

		bool bFramebufferResized = false;

		Ry::SwapChain* SwapChain;
		Ry::RenderingPlatform Platform;

		Ry::ArrayList<Ry::Delegate<void, int32, int32>> ErrorCallbacks;
		Ry::ArrayList<Ry::Delegate<void, int32, int32>> WindowResizedDelegates;
		Ry::ArrayList<Ry::Delegate<void, int32, KeyAction, int32>> KeyCallbacks;
		Ry::ArrayList<Ry::Delegate<void, int32, bool>> MouseButtonCallbacks;
		Ry::ArrayList<Ry::Delegate<void, double, double>> ScrollCallbacks;
		Ry::ArrayList<Ry::Delegate<void, uint32>> CharacterCallbacks;

		int32 FullscreenMonitor;
		int32 WindowPosBuffer[2];
		int32 WindowSizeBuffer[2];

		// GLFW window
		::GLFWwindow* WindowResource;

		/**
		 * Creates the internal platform-specific rendering context.
		 */
		void CreateSwapChain();
		void SetupWindowHints();

		// Static functionality/data
		static Window* FindWindow(::GLFWwindow* Window);
		static ::GLFWmonitor* GetMonitorByIndex(int32 index);
		
		static void WindowSizeCallback(::GLFWwindow* Window, int NewWidth, int NewHeight);
		static void FrameBufferSizeCallback(::GLFWwindow* Window, int NewWidth, int NewHeight);
		static void KeyCallback(::GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);
		static void MouseButtonCallback(::GLFWwindow* Window, int Button, int Action, int Mods);
		static void ScrollCallback(::GLFWwindow* Window, double XOff, double YOff);
		static void CharacterEntryCallback(::GLFWwindow* Window, unsigned int Codepoint);
		static void CursorPosCallback(::GLFWwindow* Window, double PosX, double PosY);
		static void WindowPosCallback(::GLFWwindow* Window, int32 PosX, int32 PosY);
		static void PathDropCallback(::GLFWwindow* Window, int32 PathCount, const char* Paths[]);

		typedef void (*GLFWdropfun)(GLFWwindow*, int, const char* []);

	};

}
