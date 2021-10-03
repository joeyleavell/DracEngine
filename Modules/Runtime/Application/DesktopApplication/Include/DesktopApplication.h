#pragma once

#include "Application.h"
#include "Core/Core.h"
#include "Core/String.h"
#include "DesktopApplicationGen.h"

struct GLFWwindow;
struct GLFWmonitor;

namespace Ry
{
	enum class RenderingPlatform;
	enum class KeyAction;

	class Window;

	struct WindowConfig
	{
		String title;
		int32 width;
		int32 height;
		bool fullscreen;
		int32 monitor;
	};

	struct DesktopConfig
	{
		int fps_lock;
		bool busy;
		Ry::String AppName;
		AbstractGame* game;
		WindowConfig window;

		DesktopConfig();
		DesktopConfig(const DesktopConfig& config);

		DesktopConfig& operator=(const DesktopConfig& config);

	};

	class DESKTOPAPPLICATION_MODULE DesktopApp : public Application
	{
	public:
		
		DesktopApp(AbstractGame* game, Ry::RenderingPlatform Platform, Ry::String app_name);
		virtual ~DesktopApp();

		/**
		 * Functions from application interface
		 */
		void Run() override;
		void Stop() override;
		int32 GetViewportWidth() const override;
		int32 GetViewportHeight() const override;
		Ry::SwapChain* GetSwapChain() const override;
		void BeginFrame() override;
		void EndFrame() override;

		int32 GetAverageFPS() const override;

	private:

		void OnEvent(const Event& Ev);
		void OnMouseButtonPressed(int32 Key, bool bPressed);
		void OnKeyPressed(int32 Key, Ry::KeyAction Action, int32 Mods);
		void OnCharPressed(uint32 Codepoint);
		void OnScroll(double ScrollX, double ScrollY);
		void OnWindowResized(int32 NewWidth, int32 NewHeight);

		bool InitWindow(const DesktopConfig& Conf);
		bool InitRenderAPI(const DesktopConfig& Conf);
		bool InitEngine(const DesktopConfig& Conf);
		bool InitFileSystem(const DesktopConfig& Conf);

		void LoadConfig(const Ry::String& Name, Ry::DesktopConfig& Conf);

		void update(float delta);
		void render();

		void Exit();

		// GLFW Event functions

	private:
		// Backup when transitioning between fullscreen/windowed

		int FrameCounter;
		int AverageFPS;
		bool Initialized;

		AbstractGame* Game;

		Ry::Window* GameWindow;
	};

}