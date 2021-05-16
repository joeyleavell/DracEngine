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
#include "Input.h"
#include "Widget/Label.h"
#include "VectorFontAsset.h"
#include "Interface/RenderCommand.h"

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
			InitWindow();
			Ry::InitRenderingEngine();
			InitAssetSystem();

			Cmd = Ry::RendAPI->CreateCommandBuffer(Wnd->GetSwapChain());

			Delegate<void, int32, int32> Resized;
			Resized.BindMemberFunction(this, &ContentBrowser::Resize);
			Wnd->AddWindowResizedDelegate(Resized);
			//OnWindowResize.AddMemberFunction();

			// Init input
			Ry::input_handler = new Ry::InputHandler;

			VectorFontAsset* Font = Ry::AssetMan->LoadAsset<VectorFontAsset>("/Engine/Fonts/arial.ttf", "font/truetype");
			BitmapFont* RTFont = Font->GenerateBitmapFont(30);
			
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

			ShapeBatch = new Batch(Wnd->GetSwapChain(), Wnd->GetSwapChain()->GetDefaultRenderPass(), UIShader, false);
			TextBatch = new Batch(Wnd->GetSwapChain(), Wnd->GetSwapChain()->GetDefaultRenderPass(), UIText, true);
			TextureBatch = new Batch(Wnd->GetSwapChain(), Wnd->GetSwapChain()->GetDefaultRenderPass(), UITexture, true);

			//TextureBatch->AddItem(TextureItem, Tex);
			//TextureBatch->Update();
			//TextureBatch->Render();

			// Create UI
			UI = new UserInterface;
			UI->SetShapeBatch(ShapeBatch);
			UI->SetTextBatch(TextBatch);
			UI->SetTextureBatch(TextureBatch);

			Ry::BorderWidget* Canvas;
			
			NewWidgetAssign(Canvas, BorderWidget)
			.FillX(1.0f)
			.FillY(1.0f)
			.SetHAlign(HAlign::CENTER)
			.SetVAlign(VAlign::BOTTOM)
			[
				NewWidget(Ry::BorderWidget)
				.DefaultBox(WHITE.ScaleRGB(0.1f), GREEN, 5, 0)
				.HoveredBox(WHITE.ScaleRGB(0.05f), GREEN, 5, 0)
				.Padding(10.0f, 10.0f)
				.FillX(1.0f)
				[
					NewWidgetAssign(Grid, Ry::GridLayout)
					.SetCellSize(100.0f)
				]
			];


			for(int32 Slot = 0; Slot < 16; Slot ++)
			{				
				Grid->AppendSlot(
					NewWidget(Ry::VerticalLayout)
					+
					NewWidget(Ry::BorderWidget)
					.DefaultImage(Tex)
					.HoveredImage(Tex, WHITE.ScaleRGB(0.5f))
					.Padding(30.0f, 30.0f)
					+
					NewWidget(Ry::Label)
					.SetText("Test")
					.SetStyle(RTFont, WHITE)
				);
			}

			UI->AddRoot(*Canvas);

			//Wnd->GetSwapChain()->OnSwapChainDirty.AddMemberFunction(this, &ContentBrowser::SwapChainDirty);
		}

		void Resize(int32 Width, int32 Height)
		{
			// todo: cant re record primary command buffer here unless we explicitly re-record secondary buffers
			// the below just marks the secondaries for needing recording

			ShapeBatch->SetRenderPass(Wnd->GetSwapChain()->GetDefaultRenderPass());
			TextureBatch->SetRenderPass(Wnd->GetSwapChain()->GetDefaultRenderPass());
			TextBatch->SetRenderPass(Wnd->GetSwapChain()->GetDefaultRenderPass());

			ShapeBatch->Resize(Width, Height);
			TextureBatch->Resize(Width, Height);
			TextBatch->Resize(Width, Height);

			UI->Draw();

			ShapeBatch->Render();
			TextureBatch->Render();
			TextBatch->Render();

			RecordCmds();
		}

		void Update()
		{
			Ry::input_handler->setMouse(Wnd->GetCursorX(), Wnd->GetCursorY());
			
			MouseEvent Ev;
			Ev.Type = EVENT_MOUSE;
			Ev.MouseX = Ry::input_handler->getX();
			Ev.MouseY = Ry::input_handler->getY();
			Ev.MouseDeltaX = Ry::input_handler->getDx();
			Ev.MouseDeltaY = Ry::input_handler->getDy();

			UI->OnEvent(Ev);

		}

		void RecordCmds()
		{
			Cmd->BeginCmd();
			{
				Cmd->BeginRenderPass(Wnd->GetSwapChain()->GetDefaultRenderPass());
				{

					// Only draw first 10 layers
					for (int32 Layer = 0; Layer < 10; Layer++)
					{
						CommandBuffer* Shape = ShapeBatch->GetCommandBuffer(Layer);
						CommandBuffer* Text = TextBatch->GetCommandBuffer(Layer);
						CommandBuffer* Texture = TextureBatch->GetCommandBuffer(Layer);

						if (Shape)
						{
							Cmd->DrawCommandBuffer(Shape);
						}
						
						if (Texture)
						{
							Cmd->DrawCommandBuffer(Texture);
						}
						
						if (Text)
						{
							Cmd->DrawCommandBuffer(Text);
						}
					}

				}
				Cmd->EndRenderPass();
			}
			Cmd->EndCmd();
		}

		void Render()
		{
			Wnd->Update();

			ShapeBatch->Render();
			TextureBatch->Render();
			TextBatch->Render();

			RecordCmds();
			
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
		Ry::GridLayout* Grid;

		SharedPtr<BatchItem> TextureItem;

		Ry::CommandBuffer* Cmd;

		Ry::Batch* ShapeBatch;
		Ry::Batch* TextBatch;
		Ry::Batch* TextureBatch;

		Ry::Window* Wnd;

		UserInterface* UI;

	};
	
}
