#pragma once

#include "Core/Core.h"
#include "Core/String.h"
#include "ApplicationGen.h"

namespace Ry
{

	class APPLICATION_MODULE ScreenSizeListener
	{
	public:
		virtual void resized(int new_width, int new_height) = 0;
	};

	/**
	 * Abstract class that should be overridden by applications.
	 */
	class APPLICATION_MODULE AbstractGame
	{

	public:

		virtual ~AbstractGame() = default;

		/**
		 * Initializes the application. All resources should be allocated in this function.
		 */
		virtual void Init() = 0;

		/**
		 * Called each frame when the application should update. Only application logic should occur in this function. The frequency of this is defined in the application and is platform dependent.
		 */
		virtual void Update(float Delta) = 0;

		/**
		 * Called each frame when the application should render. The frequency of this is defined in the application and is platform dependent.
		 */
		virtual void Render() = 0;

		/**
		 * The application should free its resources and perform necessary cleanup in this function.
		 */
		virtual void Quit() = 0;

	};

	/**
	 * Base application class for all platforms.
	 */
	class APPLICATION_MODULE Application
	{
	public:

		/**
		 * Creates a new application with the specified abstract game.
		 *
		 * @param game The game that will be used for the application.
		 */
		Application(AbstractGame* Game, Ry::String AppName) : Running(false), ApplicationName(AppName) {};

		/**
		 * Frees allocated resources.
		 */
		virtual ~Application() = default;

		/**
		 * Starts the application synchronously. This function will exit whenever the application has been successfully terminated.
		 */
		virtual void Run() = 0;

		/**
		 * Sends the command to synchronously terminate this application. The current frame will be the last frame.
		 */
		virtual void Stop() = 0;

		/**
		 * @return int32 The width of the device screen in pixels.
		 */
		virtual int32 GetViewportWidth() const = 0;

		/**
		 * @return int32 The height of the device screen in pixels.
		 */
		virtual int32 GetViewportHeight() const = 0;

		/**
		 * @return int32 The current operating frames per second the application is achieving.
		 */
		virtual int32 GetAverageFPS() const = 0;

		// virtual bool IsFullscreen() const = 0;
		// virtual void SetFullscreen(bool fullscreen) = 0;
		// virtual void SetFullscreen(bool fullscreen, int32 monitor) = 0;

		/**
		 * @return bool True if the application is in a loop, false if not.
		 */
		bool IsRunning() const
		{
			return Running;
		}

		Ry::String GetApplicationName()
		{
			return ApplicationName;
		}
		
	protected:
		bool Running;
		Ry::String ApplicationName;
		
	};

}