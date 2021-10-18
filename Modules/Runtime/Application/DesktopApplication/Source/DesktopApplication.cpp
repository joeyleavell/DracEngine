#include "DesktopApplication.h"
#include "Core/Core.h"
#include "Audio.h"
#include "Manager/AssetManager.h"
#include "Factory/TextFileFactory.h"
#include "Factory/ObjMeshFactory.h"
#include "Factory/TrueTypeFontFactory.h"
#include "Factory/TextureFactory.h"
#include "Factory/AudioFactory.h"
#include "File/File.h"
#include "Core/Globals.h"
#include "Input.h"
#include "GLRenderAPI.h"
#include "VulkanRenderAPI.h"
#include "RenderingEngine.h"
#include "Scene.h"
#include "Automate.h"
#include "Window.h"
#include "Core/Globals.h"
#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include <fstream>
#include "Window.h"
#include "Tiled.h"

// Json
#include "nlohmann/json.hpp"
#include "Core/PlatformProcess.h"
#include "Factory/FbxFactory.h"
typedef nlohmann::json Json;

namespace Ry
{

	void DesktopApp::OnWindowResized(int NewWidth, int NewHeight)
	{
		Ry::OnWindowResize.Broadcast(NewWidth, NewHeight);
	}

	void DesktopApp::OnMouseButtonPressed(int32 Button, bool bPressed)
	{
		Ry::input_handler->setButton(Button, bPressed);
	}

	void DesktopApp::OnKeyPressed(int32 Key, Ry::KeyAction Action, int32 Mods)
	{
		if(Action == KeyAction::PRESS)
		{
			Ry::input_handler->setKey(Key, true);
		}
		else if(Action == KeyAction::RELEASE)
		{
			Ry::input_handler->setKey(Key, false);
		}
	}

	void DesktopApp::OnCharPressed(uint32 Codepoint)
	{
		Ry::input_handler->sendChar(Codepoint);
	}

	void DesktopApp::OnScroll(double ScrollX, double ScrollY)
	{
		Ry::input_handler->setScroll(ScrollY);
	}
	
	DesktopConfig::DesktopConfig()
	{
		window.title = "game";
		window.width = 800;
		window.height = 600;
		window.fullscreen = false;
		window.monitor = 0;
		busy = false;
		fps_lock = 60;
		game = nullptr;
	}

	DesktopConfig::DesktopConfig(const DesktopConfig& config)
	{
		busy = config.busy;
		fps_lock = config.fps_lock;
		window.title = config.window.title;
		window.width = config.window.width;
		window.height = config.window.height;
		window.fullscreen = config.window.fullscreen;
		window.monitor = config.window.monitor;
	}

	DesktopConfig& DesktopConfig::operator=(const DesktopConfig& config)
	{
		busy = config.busy;
		fps_lock = config.fps_lock;
		window.title = config.window.title;
		window.width = config.window.width;
		window.height = config.window.height;
		window.fullscreen = config.window.fullscreen;
		window.monitor = config.window.monitor;

		return *this;
	}

	DesktopApp::DesktopApp(AbstractGame* game, Ry::RenderingPlatform Plat, Ry::String app_name):
		Ry::Application(game, Plat, app_name)
	{
		this->AverageFPS = 0;
		this->FrameCounter = 0;
		this->Running = false;
		this->Game = game;

		// Set the global rendering platform
		Ry::rplatform = new Ry::RenderingPlatform(Plat);
	}

	DesktopApp::~DesktopApp()
	{
		delete input_handler;
	}

	void DesktopApp::Run()
	{
		Ry::DesktopConfig Conf;
		LoadConfig(ApplicationName, Conf);

		bool Success = InitEngine(Conf);

		if (Success)
		{
			Ry::Log->Log("Engine initiinitialized");
			Running = true;

			// init subsystems here

			std::chrono::high_resolution_clock::time_point last_frame = std::chrono::high_resolution_clock::now();
			std::chrono::high_resolution_clock::time_point last_second = last_frame;
			uint64 frame_period = (uint64)std::floor(1e9 / Conf.fps_lock);

			while (Running)
			{
				// Synchronize framerate 
				std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
				std::chrono::nanoseconds delta_frame = now - last_frame;
				std::chrono::nanoseconds delta_second = now - last_second;

				// Amount of nanoseconds.
				uint64 frame_time_ns = delta_frame.count();
				uint64 second_time_ns = delta_second.count();

				//if (frame_time_ns >= frame_period)
				{
					last_frame = now;

					float delta = (float)(frame_time_ns / 1e9);

					update(delta);
					render();

					FrameCounter++;
				}

				if (second_time_ns >= 1e9)
				{
					AverageFPS = FrameCounter;
					FrameCounter = 0;
					last_second = now;

					std::cout << AverageFPS << std::endl;
				}

				// Sleep the thread to reduce power usage
				//std::this_thread::sleep_for(std::chrono::milliseconds(2));
			}
		}
		else
		{
			Ry::Log->Log("Engine failed to initialize");
		}
	}

	void DesktopApp::Stop()
	{
		Running = false;
	}

	int32 DesktopApp::GetViewportWidth() const
	{
		return GameWindow->GetWindowWidth();
	}

	int32 DesktopApp::GetViewportHeight() const
	{
		return GameWindow->GetWindowHeight();
	}

	Ry::SwapChain* DesktopApp::GetSwapChain() const
	{
		return GameWindow->GetSwapChain();
	}

	void DesktopApp::BeginFrame()
	{
		GameWindow->BeginFrame();
	}

	void DesktopApp::EndFrame()
	{
		GameWindow->EndFrame();
	}

	int32 DesktopApp::GetAverageFPS() const
	{
		return AverageFPS;
	}

	void DesktopApp::OnEvent(const Event& Ev)
	{
		if(Game)
		{
			Game->OnEvent(Ev);
		}
	}

	bool DesktopApp::InitEngine(const DesktopConfig& Conf)
	{
		// Set singleton
		Ry::app = this;

		// Setup get viewport width and get viewport height delegates
		Ry::ViewportWidthDel.BindLambda([]()
		{
			return Ry::app->GetViewportWidth();
		});

		Ry::ViewportHeightDel.BindLambda([]()
		{
			return Ry::app->GetViewportHeight();
		});

		// Select rendering API here
		
		// Initialize window
		if(!InitWindow(Conf))
		{
			return false;
		}

		// Initialize file system
		if(!InitFileSystem(Conf))
		{
			Ry::Log->Log("Failed to initialize file system");
			return false;
		}

		// Initialize rendering engine.
		// J.Leavell -- TODO: remove all opengl calls out of this class!
		Ry::Log->Log("Initializing rendering engine");
		Ry::InitRenderingEngine();

		// Initialize scene
		Ry::Log->Log("Initializing scene");
		Ry::InitScene();

		// Initialize ` system
		Ry::Log->Log("Initializing audio");
		Ry::InitAudio();

		// Initialize input system
		Ry::Log->Log("Initializing input");
		Ry::input_handler = new Ry::InputHandler;

		if (Game)
		{
			Ry::Log->Log("Initializing game");
			Game->Init();

			// Send initial window resize event
			//WindowSizeCallback(window, GetViewportWidth(), GetViewportHeight());
		}
		else
		{
			Ry::Log->Log("Game was nullptr");
			return false;
		}

		return true;
	}

	bool DesktopApp::InitFileSystem(const DesktopConfig& Conf)
	{
		
#ifndef SHIPPING_BUILD

		// Reload engine content
		//Ry::ReloadResourceDirectory("../../../Engine/RyRuntime/Resources", "./Engine/");
		
		// Reload game content
		//Ry::ReloadResourceDirectory("../../../Projects/" + Conf.AppName + "/Resources", "./Content/");
		
		// Make default data directory for the game if it does not already exist
		//if (!Ry::File::DoesFileExist("./Data"))
			//Ry::File::MakeDirectories("./Data");
#endif

		// Mount virtual file system directories depending on if we're distributing
#ifdef RYBUILD_DISTRIBUTE
		Ry::String ModuleRoot = Ry::File::GetParentPath(GetPlatformModulePath());
		Ry::String ResourcesRoot = Ry::File::Join(ModuleRoot, "Resources");
		Ry::String ShadersRoot = Ry::File::Join(ModuleRoot, "Shaders");
		Ry::String EngineResRoot = Ry::File::Join(ResourcesRoot, "Engine");
		Ry::String GameResRoot = Ry::File::Join(ResourcesRoot, "Game");
		Ry::String DataRoot = Ry::File::Join(ModuleRoot, "Data");

		Ry::File::MountDirectory(EngineResRoot, "Engine");
		Ry::File::MountDirectory(ShadersRoot, "Shaders");
		Ry::File::MountDirectory(GameResRoot, "Content");
		Ry::File::MountDirectory(DataRoot, "Data");

#else
		Ry::String EngineRoot = Ry::File::GetParentPath(Ry::File::GetParentPath(GetPlatformModulePath()));
		Ry::String ResourcesRoot = Ry::File::Join(EngineRoot, "Resources");
		Ry::String ShadersRoot = Ry::File::Join(EngineRoot, "Shaders");

		Ry::File::MountDirectory(ResourcesRoot, "Engine");

		// Todo: support custom game shaders
		Ry::File::MountDirectory(ShadersRoot, "Shaders");

		// Allow the editor to mount the project directories
#endif

		// Initialize asset manager
		AssetMan = new Ry::AssetManager;

		// Register asset factories
		// TODO: these should be moved
		AssetMan->RegisterFactory("text", new Ry::TextFileFactory);
		AssetMan->RegisterFactory("mesh/obj", new ObjMeshFactory);
	//	AssetMan->RegisterFactory("mesh/fbx", new FbxFactory);
		AssetMan->RegisterFactory("font/truetype", new TrueTypeFontFactory);
		AssetMan->RegisterFactory("image", new TextureFactory);
		AssetMan->RegisterFactory("sound", new AudioFactory);
		AssetMan->RegisterFactory("tmx", new TmxMapFactory);

		// TODO: Load default engine assets here...

		return true;
	}

	bool DesktopApp::InitWindow(const DesktopConfig& Conf)
	{
		// Initialize the windowing subsystem
		if(!InitWindowing())
		{
			Ry::Log->LogError("Failed to initialize windowing subsystem");
		}

		// Initialize the rendering API
		if (!InitRenderAPI(Conf))
		{
			return false;
		}
		
		GameWindow = new Window(*Ry::rplatform);

		if(!GameWindow->CreateWindow(*Conf.window.title, Conf.window.width, Conf.window.height))
		{
			Ry::Log->Log("Failed to create window");
			return false;
		}

		// Setup window callbacks
		GameWindow->AddKeyPressDelegate(Ry::Delegate<void, int32, KeyAction, int32>::CreateMemberFunctionDelegate(this, &DesktopApp::OnKeyPressed));
		GameWindow->AddMouseButtonDelegate(Ry::Delegate<void, int32, bool>::CreateMemberFunctionDelegate(this, &DesktopApp::OnMouseButtonPressed));
		GameWindow->AddWindowResizedDelegate(Ry::Delegate<void, int32, int32>::CreateMemberFunctionDelegate(this, &DesktopApp::OnWindowResized));
		GameWindow->AddKeyCharDelegate(Ry::Delegate<void, uint32>::CreateMemberFunctionDelegate(this, &DesktopApp::OnCharPressed));
		GameWindow->AddScrollDelegate(Ry::Delegate<void, double, double>::CreateMemberFunctionDelegate(this, &DesktopApp::OnScroll));

		GameWindow->OnEvent.AddMemberFunction(this, &DesktopApp::OnEvent);

		// todo: make this platform agnostic
		//GameWindow->SetVSyncEnabled(false);
	
		return true;
	}

	bool DesktopApp::InitRenderAPI(const DesktopConfig& Conf)
	{
		// Initialize the different rendering APIs here
		
		if(*Ry::rplatform == RenderingPlatform::OpenGL)
		{
			if (!Ry::InitOGLRendering())
			{
				Ry::Log->Log("Failed to initialize OpenGL");
				return false;
			}
		}
		else if (*Ry::rplatform == RenderingPlatform::Vulkan)
		{
			if (!Ry::InitVulkanAPI())
			{
				Ry::Log->Log("Failed to initialize Vulkan");
				return false;
			}
		}

		return true;
	}

	void DesktopApp::LoadConfig(const Ry::String& Name, Ry::DesktopConfig& Conf)
	{
		// Create a default config
		Conf.AppName = Name;
		Conf.busy = false;
		Conf.fps_lock = 60;
		Conf.game = nullptr;
		Conf.window.title = "My Game";
		Conf.window.width = 800;
		Conf.window.height = 600;
		Conf.window.fullscreen = false;
		Conf.window.monitor = 0;

		Ry::String FileName = Name + ".project";

		if (Ry::File::DoesFileExist(FileName))
		{
			Ry::String FileContents = Ry::File::LoadFileAsString2(FileName);
			Json ProjectJson = Json::parse(*FileContents);

			ProjectJson["FrameLock"].get_to(Conf.fps_lock);
			ProjectJson["Window"]["Width"].get_to(Conf.window.width);
			ProjectJson["Window"]["Height"].get_to(Conf.window.height);
			ProjectJson["Window"]["Fullscreen"]["Enabled"].get_to(Conf.window.fullscreen);
			ProjectJson["Window"]["Fullscreen"]["Monitor"].get_to(Conf.window.monitor);
		}
		else
		{
			// Generate and save new application launch settings
			Json ProjectJson;
			
			ProjectJson["FrameLock"] = Conf.fps_lock;
			ProjectJson["Window"]["Width"] = Conf.window.width;
			ProjectJson["Window"]["Height"] = Conf.window.height;
			ProjectJson["Window"]["Fullscreen"]["Enabled"] = Conf.window.fullscreen;
			ProjectJson["Window"]["Fullscreen"]["Monitor"] = Conf.window.monitor;

			// Dump json string to project launch file
			std::ofstream Output (*FileName);
			Output << ProjectJson.dump(4);
			Output.close();
		}
	}

	void DesktopApp::update(float delta)
	{
		// Detect if the user has requested the window to close
		if (GameWindow->ShouldClose())
		{
			Stop();
		}

		GameWindow->Update();

		// Update the mouse cursor
		double CursorX, CursorY;
		GameWindow->GetCursorPos(CursorX, CursorY);
		input_handler->setMouse((float) CursorX, (float) CursorY);

		// Update audio
		Ry::UpdateAudio();

		// Fire game update
		Game->Update(delta);
	}

	void DesktopApp::render()
	{		
		Game->Render();
		//GameWindow->SwapBuffers();
	}

	void DesktopApp::Exit()
	{
		// Shutdown game
		Ry::Log->Log("Shutting down");
		Game->Quit();
		delete Game;
		
		// Shutdown asset manager
		AssetMan->UnloadAll();
		delete AssetMan;

		// TODO: Shut down file system

		// TODO: Shut down rendering system
		Ry::QuitRenderingEngine();

		// Shut down window
		Ry::Log->Log("Closing window");
		GameWindow->Destroy();
	}

}