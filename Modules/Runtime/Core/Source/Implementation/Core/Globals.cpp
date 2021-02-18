#include "Core/Globals.h"
//#include "Core/Application/DesktopApplication.h"
#include <iostream>

namespace Ry
{
	Ry::Logger* Log = nullptr;
	Ry::Renderer* RenderAPI = nullptr;

	Ry::String ResourcesDirectory = ".";
	
	MulticastDelegate<int32, int32> OnWindowResize;
	Ry::Application* app = nullptr;
	Ry::Delegate<int32> ViewportWidthDel;
	Ry::Delegate<int32> ViewportHeightDel;
	Ry::InputHandler* input_handler = nullptr;
	//Ry::IAssetManager* AssetMan = nullptr;
	//Ry::Renderer* RenderAPI = nullptr;
	Ry::RenderingPlatform* rplatform = nullptr;// = RenderingPlatform::GL4;

	int32 GetViewportWidth()
	{
		return ViewportWidthDel.Execute();
	}
	
	int32 GetViewportHeight()
	{
		return ViewportHeightDel.Execute();
	}
	
}