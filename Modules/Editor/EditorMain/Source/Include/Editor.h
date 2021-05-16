#pragma once
#include "Core/Platform.h"
#include "Core/Globals.h"
#include "Manager/AssetManager.h"
#include "Factory/ObjMeshFactory.h"
#include "Factory/TrueTypeFontFactory.h"
#include "Factory/TextureFactory.h"
#include "Factory/AudioFactory.h"
#include "GLRenderAPI.h"
#include "RenderingEngine.h"
#include "Window.h"
#include "VectorFontAsset.h"
#include "TextureAsset.h"
#include "Widget/Label.h"
#include "SwapChain.h"
#include "Widget/UserInterface.h"
#include "Factory/TextFileFactory.h"
#include "Widget/BorderWidget.h"
#include "Widget/GridLayout.h"
#include "Widget/VerticalPanel.h"
#include "Input.h"
#include "EditorWindow.h"
#include "File/File.h"
#include "VulkanRenderAPI.h"
#include "Core/PlatformProcess.h"

namespace Ry
{

	class Editor
	{
	public:

		Editor(Ry::RenderingPlatform Platform)
		{
			Ry::rplatform = new Ry::RenderingPlatform(Platform);
		}

		void InitFileSystem()
		{
			Ry::String EngineRoot = Ry::File::GetParentPath(Ry::File::GetParentPath(GetPlatformModulePath()));
			Ry::String ResourcesRoot = Ry::File::Join(EngineRoot, "Resources");
			Ry::String ShadersRoot = Ry::File::Join(EngineRoot, "Shaders");

			Ry::File::MountDirectory(ResourcesRoot, "Engine");

			// Todo: Support custom game shaders as well
			Ry::File::MountDirectory(ShadersRoot, "Shaders");
		}

		void InitAssetSystem()
		{
			AssetMan = new Ry::AssetManager;

			// Register asset factories
			// TODO: these should be moved
			AssetMan->RegisterFactory("text", new TextFileFactory);
			AssetMan->RegisterFactory("mesh/obj", new ObjMeshFactory);
			AssetMan->RegisterFactory("font/truetype", new TrueTypeFontFactory);
			AssetMan->RegisterFactory("image", new TextureFactory);
			AssetMan->RegisterFactory("sound", new AudioFactory);
		}

		void InitRenderAPI()
		{

			if (*Ry::rplatform == Ry::RenderingPlatform::OpenGL)
			{
				if (!Ry::InitOGLRendering())
				{
					Ry::Log->LogError("Failed to initialize OpenGL");
				}
				else
				{
					Ry::Log->Log("Initialized OpenGL");
				}
			}

			if (*Ry::rplatform == Ry::RenderingPlatform::Vulkan)
			{
				if (!Ry::InitVulkanAPI())
				{
					Ry::Log->LogError("Failed to initialize Vulkan API");
					std::abort();
				}
			}

		}

		void ShutdownRenderAPI()
		{
			if (*Ry::rplatform == Ry::RenderingPlatform::Vulkan)
			{
				ShutdownVulkanAPI();
			}
		}

		void InitLogger()
		{
			Ry::Log = new Ry::Logger;
		}

		void Init()
		{
			InitLogger();
			InitFileSystem();
			InitWindowing();
			InitRenderAPI();
			InitAssetSystem();

			PrimaryWindow = new EditorWindow;
			PrimaryWindow->Init();

			// Startup initial editor window

			//Wnd->GetSwapChain()->OnSwapChainDirty.AddMemberFunction(this, &ContentBrowser::SwapChainDirty);
		}

		void Update()
		{


		}

		void Run()
		{
			Init();

			while (!PrimaryWindow->WantsClose())
			{
				PrimaryWindow->Update();
				PrimaryWindow->Render();
			}
		}

	private:

		EditorWindow* PrimaryWindow;

	};
	
}
