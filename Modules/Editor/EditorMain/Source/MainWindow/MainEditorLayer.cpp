#include "MainWindow/MainEditorLayer.h"

#include "DetailsView.h"
#include "RenderAPI.h"
#include "Widget/UserInterface.h"
#include "Widget/BorderWidget.h"
#include "VectorFontAsset.h"
#include "Manager/AssetManager.h"
#include "TextureAsset.h"
#include "Widget/Layout/VerticalPanel.h"
#include "Widget/Label.h"
#include "Widget/Layout/Splitter.h"
#include "RenderCommand.h"
#include "ContentBrowser/ContentBrowserWidget.h"
#include "WidgetManager.h"
#include "EditorStyle.h"
#include "File/Serializer.h"
#include "RenderingResource.h"
#include "Package/Package.h"
#include "RenderPass.h"
#include "MainWindow/SceneView.h"

namespace Ry
{
	Texture* Avocado{};

	MainEditorLayer::MainEditorLayer(SwapChain* Parent):
	Layer(Parent)
	{
		// Create the editor style
		InitializeEditorStyle();

		Avocado = Ry::AssetMan->GetOrLoadAsset<TextureAsset>("/Engine/Textures/avocado.png", "image")->CreateRuntimeTexture();

		//Color = OffScreenDesc.AddColorAttachment(); // Extra color attachment
		SwapColor = OnScreenDesc.AddSwapChainColorAttachment(ParentSC);
		SwapDepth = OnScreenDesc.AddSwapChainDepthAttachment(ParentSC);

		// Create off-screen pass. Shouldn't need to be re-created on re-size.
		//OffScreenPass = Ry::RendAPI->CreateRenderPass();
		//OffScreenPass->SetFramebufferDescription(OffScreenDesc);
		{
		//	int32 OffScreenSub = OffScreenPass->CreateSubpass();
		//	OffScreenPass->AddSubpassAttachment(OffScreenSub, Color);
		}
		//OffScreenPass->CreateRenderPass();

		// Main pass. This needs to be re-created on resize.
		CreateRenderPass();

		// Create on and off screen framebuffers
		//OffScreenBuffer = Ry::RendAPI->CreateFrameBuffer(ParentSC->GetSwapChainWidth(), ParentSC->GetSwapChainHeight(), OffScreenPass, OffScreenDesc);
		OnScreenBuffer = Ry::RendAPI->CreateFrameBuffer(ParentSC->GetSwapChainWidth(), ParentSC->GetSwapChainHeight(), MainPass, OnScreenDesc);

		// Initialize primary command buffer
		Cmd = Ry::RendAPI->CreateCommandBuffer(Parent);
		//OffScreenBatch = new Batch(ParentSC, OffScreenPass);

		// Compile impose shader, re-use Vertex/Shape vertex shader
		Ry::CompileShader("Impose", "Vertex/Impose", "Fragment/Impose");

		OnScreenBatch= new Batch(ParentSC, MainPass);
		OnScreenBatch->AddPipeline("Impose", "Impose");

		//OnScreenBatch2 = new Batch(ParentSC, MainPass);
		//OnScreenBatch2->AddPipeline("Impose", "Impose");

		// Create a full-texture rect
		FullScreenRect = MakeItem();
		Ry::BatchTexture(FullScreenRect, WHITE, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 2.0f, 2.0f, 0.0f);

		TestWorld = new World2D(Parent);
		// Create scene renderer, using a temporary width and height
		SceneRenderer = new Scene2DRenderer(TestWorld->GetScene(), ParentSC, 1080, 720);

		SharedPtr<TestEntity> NewEnt = MakeShared(new TestEntity(TestWorld));
		TestWorld->AddEntity(NewEnt);

		// Create UI
		UI = new UserInterface(OnScreenBatch, GetStyle("Editor"));

		Ry::SharedPtr<Ry::BorderWidget> Root;
		Ry::SharedPtr<Ry::ContentBrowserWidget> BrowserWidget;
		Ry::SharedPtr<Ry::ScrollPane> DetailsPanel;
		SharedPtr<Ry::SceneView> SceneView;

		// Build editor UI
		Root = LoadWidget<Ry::BorderWidget>("/Engine/UI/EditorMain.ui");
		if(Root.IsValid())
		{
			BrowserWidget = Root->FindChildWidget<Ry::ContentBrowserWidget>("BrowserWidget");

			if(SceneView = Root->FindChildWidget<Ry::SceneView>("SceneView"))
			{
				SceneView->SetSceneRenderer(SceneRenderer);
				SceneView->OnSceneResized.AddMemberFunction(this, &MainEditorLayer::SceneResized);
			}

			DetailsPanel = Root->FindChildWidget<Ry::ScrollPane>("DetailsPanel");
		}
		
		// Create the content browser utility
		ContentBrowse = new ContentBrowser(BrowserWidget);

		UI->AddRoot(Root);

		if(SceneView)
		{
			SizeType Canvas = SceneView->ComputeSize();
			TestWorld->Resize(Canvas.Width, Canvas.Height);
		}

		if(SceneView)
		{
			SizeType SceneViewSize = SceneView->ComputeSize();
		}

		if(DetailsPanel)
		{
			TestObj Obj;
			Obj.TestString = "hello";
			Ry::SharedPtr<DetailsView> Details = MakeShared(new DetailsView);
			Details->SetObject(&Obj);
			DetailsPanel->AppendSlot(Details);
		}

		// Initialize the directory now that the content browser is added to the root
		ContentBrowse->SetDirectory("/Engine");

		// Add the item to the batch and update
		//UpdateFullScreenRect();

	}

	void MainEditorLayer::Update(float Delta)
	{
		TestWorld->Update(Delta);

		UI->Update();
	}

	void MainEditorLayer::Render()
	{
		//OffScreenBatch->Render();
		OnScreenBatch->Render();
		SceneRenderer->Render(*TestWorld->GetCamera());
		//OnScreenBatch2->Render();

		RecordCmds();

		Cmd->Submit();

		//TestWorld->Draw();
	}

	bool MainEditorLayer::OnEvent(const Event& Ev)
	{
		//return false;
		return UI->OnEvent(Ev);
	}

	void MainEditorLayer::OnResize(int32 Width, int32 Height)
	{
		// todo: cant re record primary command buffer here unless we explicitly re-record secondary buffers
		// the below just marks the secondaries for needing recording

		// Re-size framebuffers
		//OffScreenBuffer->Recreate(Width, Height, )A

		// Re-create the render pass
		CreateRenderPass();

		// Re-create the framebuffer
		OnScreenBuffer->Recreate(Width, Height, MainPass);
		SceneRenderer->Resize(Width, Height);

		//UpdateFullScreenRect();

		//OffScreenBuffer->Recreate(Width, Height, OffScreenPass);

		//OffScreenBatch->SetRenderPass(OffScreenPass);
		//OffScreenBatch->Resize(Width, Height);

		OnScreenBatch->SetRenderPass(MainPass);
		OnScreenBatch->Resize(Width, Height);

		//OnScreenBatch2->SetRenderPass(MainPass);
		//OnScreenBatch2->Resize(Width, Height);

		//TestWorld->Resize(Width, Height);

		//UI->FullRefresh();
		UI->Redraw();
		
	}

	void MainEditorLayer::SceneResized(uint32 Width, uint32 Height)
	{
		// Update camera
		TestWorld->Resize(Width, Height);
	}

	void MainEditorLayer::UpdateFullScreenRect()
	{
		// Refreshes the color attachment, used when screen gets resized and framebuffer is re-createdq
		OnScreenBatch->RemoveItem(FullScreenRect);
		OnScreenBatch->AddItem(FullScreenRect, "Impose", PipelineState{}, SceneRenderer->GetSceneTexture(), 20);
		OnScreenBatch->Update();
	}

	void MainEditorLayer::CreateRenderPass()
	{
		// Delete the current one if it exists
		if (MainPass)
		{
			MainPass->DeleteRenderPass();
			delete MainPass;
		}

		MainPass = Ry::RendAPI->CreateRenderPass();
		MainPass->SetFramebufferDescription(OnScreenDesc);
		{
			int32 OnScreenPass = MainPass->CreateSubpass();
			MainPass->AddSubpassAttachment(OnScreenPass, SwapColor);
			MainPass->AddSubpassAttachment(OnScreenPass, SwapDepth);
		}
		MainPass->CreateRenderPass();

	}

	void MainEditorLayer::RecordCmds()
	{
		Cmd->Reset();
		
		Cmd->BeginCmd();
		{
			// Render scene
			SceneRenderer->DrawCommands(Cmd);

			// Composite off-screen into the on-screen buffer
			Cmd->BeginRenderPass(MainPass, OnScreenBuffer, true);
			{
				//OnScreenBatch->DrawCommandBuffers(Cmd);
				OnScreenBatch->DrawCommandBuffers(Cmd);
			}
			Cmd->EndRenderPass();
		}
		Cmd->EndCmd();
	}
	
}
