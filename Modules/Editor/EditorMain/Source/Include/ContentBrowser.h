#pragma once

#include "Window.h"
#include "Core/Engine.h"
#include "Manager/AssetManager.h"
#include "File/File.h"
#include "Factory/TextFileFactory.h"
#include "Factory/ObjMeshFactory.h"
#include "Factory/TrueTypeFontFactory.h"
#include "Factory/TextureFactory.h"
#include "Factory/AudioFactory.h"
#include "Core/PlatformProcess.h"
#include "GLRenderAPI.h"
#include "VulkanRenderAPI.h"
#include "Core/Globals.h"
#include "Widget/HorizontalPanel.h"
#include "Widget/GridLayout.h"
#include "TextureAsset.h"

namespace Ry
{

	class ContentBrowser
	{
	public:

		ContentBrowser(Ry::RenderingPlatform Platform)
		{
			Ry::rplatform = new Ry::RenderingPlatform(Platform);
			Wnd = nullptr;
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
			AssetMan->RegisterFactory("text", new Ry::TextFileFactory);
			AssetMan->RegisterFactory("mesh/obj", new ObjMeshFactory);
			AssetMan->RegisterFactory("font/truetype", new TrueTypeFontFactory);
			AssetMan->RegisterFactory("image", new TextureFactory);
			AssetMan->RegisterFactory("sound", new AudioFactory);
		}

		int32 GetViewportWidth()
		{
			return Wnd->GetWindowWidth();
		}

		int32 GetViewportHeight()
		{
			return Wnd->GetWindowHeight();
		}

		void InitWindow()
		{
			Wnd = new Window(*Ry::rplatform);

			if (!Wnd->CreateWindow("Editor", 1080, 720))
			{
				Ry::Log->Log("Failed to create a window");
			}
			else
			{
				Ry::Log->Log("Window created");
			}

			Ry::ViewportWidthDel.BindMemberFunction(this, &ContentBrowser::GetViewportWidth);
			Ry::ViewportHeightDel.BindMemberFunction(this, &ContentBrowser::GetViewportHeight);

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

			Ry::InitRenderingEngine();

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
			InitWindow();

			TextureAsset* Asset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/Icon.png", "image");
			Texture* Tex = Asset->CreateRuntimeTexture();

			TextureItem = MakeItem();
			BatchTexture(TextureItem, WHITE, 
				350.0f, 350.0f,
				0.0f, 0.0f, 
				1.0f, 1.0f, 
				0.5f, 0.5f, 
				100.0f, 100.0f, 
				0.0f
			);

			Shader* UIShader = GetShader("Shape");
			Shader* UIText = GetShader("Font");
			Shader* UITexture = GetShader("Texture");

			ShapeBatch = new Batch(Wnd->GetSwapChain(), Wnd->GetSwapChain()->GetDefaultRenderPass(), VF1P1C, UIShader, false);
			TextBatch = new Batch(Wnd->GetSwapChain(), Wnd->GetSwapChain()->GetDefaultRenderPass(), VF1P1UV1C, UIText, true);
			TextureBatch = new Batch(Wnd->GetSwapChain(), Wnd->GetSwapChain()->GetDefaultRenderPass(), VF1P1UV1C, UITexture, true);

			TextureBatch->AddItem(TextureItem, Tex);
			TextureBatch->Update();
			TextureBatch->Render();

			// Create UI
			UI = new UserInterface;
			UI->SetShapeBatch(ShapeBatch);
			UI->SetTextBatch(TextBatch);

			Ry::BoxWidget* Canvas;

			Ry::GridLayout* Grid;
			
			NewWidgetAssign(Canvas, BoxWidget)
			.FillX(1.0f)
			.FillY(1.0f)
			.SetHAlign(HAlign::CENTER)
			.SetVAlign(VAlign::CENTER)
			[
				NewWidget(Ry::BoxWidget)
				.Padding(10.0f, 10.0f)
				.DefaultBox(GREEN, GREEN, 5, 0)
				[
					NewWidgetAssign(Grid, Ry::GridLayout)
				]
			];


			for(int32 Slot = 0; Slot < 1; Slot ++)
			{				
				Grid->AppendSlot(
					NewWidget(Ry::BoxWidget)
					.Padding(20.0f, 20.0f)
					.DefaultBox(RED, RED, 5, 0)
				);
			}

			UI->AddRoot(*Canvas);

			ShapeBatch->Render();

			Cmd = Ry::RendAPI->CreateCommandBuffer(Wnd->GetSwapChain());
			Cmd->BeginCmd();
			{
				Cmd->BeginRenderPass(Wnd->GetSwapChain()->GetDefaultRenderPass());
				{

					// Only draw first 10 layers
					for(int32 Layer = 0; Layer < 10; Layer++)
					{
						CommandBuffer* Shape = ShapeBatch->GetCommandBuffer(Layer);
						CommandBuffer* Text = TextBatch->GetCommandBuffer(Layer);
						CommandBuffer* Texture = TextureBatch->GetCommandBuffer(Layer);

						if(Shape)
						{
							Cmd->DrawCommandBuffer(Shape);
						}

						if(Text)
						{
							Cmd->DrawCommandBuffer(Text);
						}

						if(Texture)
						{
							Cmd->DrawCommandBuffer(Texture);
						}
					}
					
				}
				Cmd->EndRenderPass();
			}
			Cmd->EndCmd();
		}

		void Update()
		{
			
		}

		void Render()
		{
			Wnd->Update();

			Wnd->BeginFrame();
			{
				Cmd->Submit();
			}
			Wnd->EndFrame();
		}

		void Run()
		{
			Init();
			
			while (!Wnd->ShouldClose())
			{
				Update();
				Render();
			}
		}

	private:

		SharedPtr<BatchItem> TextureItem;

		Ry::CommandBuffer* Cmd;

		Ry::Batch* ShapeBatch;
		Ry::Batch* TextBatch;
		Ry::Batch* TextureBatch;

		Ry::Window* Wnd;

		UserInterface* UI;

	};
	
}
