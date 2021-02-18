#pragma once

#include "Core/String.h"
#include "Data/ArrayList.h"
#include "Data/Map.h"
#include "Algorithm/Algorithm.h"
#include "WindowingGen.h"
//#include "Core/Delegate.h"

struct GLFWwindow;
struct GLFWmonitor;

namespace Ry
{

	class WINDOWING_MODULE Window
	{
	public:

		Window();

		bool CreateWindow(int32 Width, int32 Height);
		bool CreateWindow(Ry::String Title, int32 Width, int32 Height);
		bool CreateWindow(Ry::String Title, int32 Width, int32 Height, int32 FSMonitor);

		void SetVSyncEnabled(bool bEnabled);

		void Destroy();

		int32 GetWindowWidth() const;
		int32 GetWindowHeight() const;

		bool IsFullscreen() const;
		void SetFullscreen(bool Fullscreen);
		void SetFullscreen(bool Fullscreen, int32 MonitorIndex);

		float GetCursorX();
		float GetCursorY();
		void GetCursorPos(double& X, double& Y);

		bool ShouldClose();

		void SwapBuffers();

		void Update();

		void InitContext();

		void AddWindowResizedDelegate(const Ry::Delegate<void, int32, int32>& Delegate);
		void AddKeyPressDelegate(const Ry::Delegate<void, int32, bool>& Delegate);
		void AddKeyCharDelegate(const Ry::Delegate<void, uint32>& Delegate);
		void AddMouseButtonDelegate(const Ry::Delegate<void, int32, bool>& Delegate);
		void AddScrollDelegate(const Ry::Delegate<void, double, double>& Delegate);

	private:

		Ry::ArrayList<Ry::Delegate<void, int32, int32>> ErrorCallbacks;
		Ry::ArrayList<Ry::Delegate<void, int32, int32>> WindowResizedDelegates;
		Ry::ArrayList<Ry::Delegate<void, int32, bool>> KeyCallbacks;
		Ry::ArrayList<Ry::Delegate<void, int32, bool>> MouseButtonCallbacks;
		Ry::ArrayList<Ry::Delegate<void, double, double>> ScrollCallbacks;
		Ry::ArrayList<Ry::Delegate<void, uint32>> CharacterCallbacks;

		int32 FullscreenMonitor;
		int32 WindowPosBuffer[2];
		int32 WindowSizeBuffer[2];

		// GLFW window
		::GLFWwindow* WindowResource;

		// Static functionality/data
		static Window* FindWindow(::GLFWwindow* Window);
		static Ry::Map<::GLFWwindow*, Window*> Windows;
		static ::GLFWmonitor* GetMonitorByIndex(int32 index);
		
		static void WindowSizeCallback(::GLFWwindow* Window, int NewWidth, int NewHeight);
		static void ErrorCallback(int Error, const char* Description);
		static void KeyCallback(::GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);
		static void MouseButtonCallback(::GLFWwindow* Window, int Button, int Action, int Mods);
		static void ScrollCallback(::GLFWwindow* Window, double XOff, double YOff);
		static void CharacterEntryCallback(::GLFWwindow* Window, unsigned int Codepoint);
		
	};

	
}
