#include "MainWindow/MainEditorLayer.h"
#include "Interface/RenderAPI.h"
#include "Widget/UserInterface.h"
#include "Widget/BorderWidget.h"
#include "VectorFontAsset.h"
#include "Manager/AssetManager.h"
#include "TextureAsset.h"
#include "Widget/Layout/VerticalPanel.h"
#include "Widget/Label.h"
#include "Widget/Layout/Splitter.h"
#include "Interface/RenderCommand.h"
#include "ContentBrowser/ContentBrowserWidget.h"
#include "WidgetManager.h"
#include "EditorStyle.h"
#include "File/Serializer.h"
#include "Interface/RenderingResource.h"
#include "Package/Package.h"
#include "Interface/RenderPass.h"

namespace Ry
{
	MainEditorLayer::MainEditorLayer(SwapChain* Parent):
	Layer(Parent)
	{
		// Create the editor style
		InitializeEditorStyle();

		/*std::cout << "type name " << *GetType<Ry::ArrayList<unsigned long long int>>()->Name << std::endl;
		Ry::OAPairIterator<Ry::String, const Ry::ReflectedClass*> Itr = RefDB.GetClassIterator();
		while (Itr)
		{
			const Ry::ReflectedClass* Cl = Itr.GetValue();
			std::cout << *Itr.GetKey() << std::endl;
			for (const Ry::Field& Field : Cl->Fields)
			{
				std::cout << "\t" << *Field.Name << " " << *Field.Type->Name << std::endl;
			}
			++Itr;
		}*/

		//TestRefl Object; 

		//RefDB.PrintAllReflectedClass();

		//const Ry::ReflectedClass* RefClass = GetReflectedClass("Ry::TestObj");
		//SharedPtr<Ry::Widget> LoadedWid = Ry::LoadWidget<Ry::Widget>("/Engine/UI/TestUI.ui");
		//Json AsJson = Ry::Jsonify(*LoadedWid.Get());

		//std::cout << "Jsonified: \n" << *AsJson.Stringify() << std::endl;

		//TestObj* Obj = NewObject<TestObj>("TestObject");

		/*Package* NewPackage = new Package("/Engine/TestPackage.rasset");
		NewPackage->SetObject(Obj);
		NewPackage->Save();*/

		// Load packagedf
		//Package* Loaded = LoadPackage("/Engine/avocado.rasset");
		//const Ry::Object* RootObject = Loaded->GetOrLoadRoot();

		//std::cout << "test" << std::endl;

		//TestReflection* NewRefl = GetReflectedClass("Ry::TestReflection")->CreateInstance<TestReflection>();
//		std::cout << "Value " << NewRefl->Other << std::endl;
		//TestRefl* Other = NewRefl->GetPropertyRef<TestRefl*>("Other2");
		//std::cout << "Value " << Refl.TestField << std::endl;

		FrameBufferDescription OffScreenDesc;
		int32 Color = OffScreenDesc.AddColorAttachment(); // Extra color attachment

		FrameBufferDescription OnScreenDesc;
		int32 SwapColor = OnScreenDesc.AddSwapChainColorAttachment(ParentSC);
		int32 SwapDepth = OnScreenDesc.AddSwapChainDepthAttachment(ParentSC);

		// Create main pass
		OffScreenPass = Ry::RendAPI->CreateRenderPass();
		OffScreenPass->SetFramebufferDescription(OffScreenDesc);
		{
			int32 OffScreenSub = OffScreenPass->CreateSubpass();
			OffScreenPass->AddSubpassAttachment(OffScreenSub, Color);
		}
		OffScreenPass->CreateRenderPass();

		// Main pass will use result from off-screen pass
		MainPass = Ry::RendAPI->CreateRenderPass();
		MainPass->SetFramebufferDescription(OnScreenDesc);
		{
			int32 OnScreenPass = MainPass->CreateSubpass();
			MainPass->AddSubpassAttachment(OnScreenPass, SwapColor);
			MainPass->AddSubpassAttachment(OnScreenPass, SwapDepth);
		}
		MainPass->CreateRenderPass();

		// Create on and off screen framebuffers
		OffScreenBuffer = Ry::RendAPI->CreateFrameBuffer(ParentSC->GetSwapChainWidth(), ParentSC->GetSwapChainHeight(), OffScreenPass, OffScreenDesc);
		OnScreenBuffer = Ry::RendAPI->CreateFrameBuffer(ParentSC->GetSwapChainWidth(), ParentSC->GetSwapChainHeight(), MainPass, OnScreenDesc);

		// Initialize primary command buffer
		Cmd = Ry::RendAPI->CreateCommandBuffer(Parent);
		Bat = new Batch(ParentSC, OffScreenPass);

		// Create an off-screen pass with one color attachment in a single sub-pass
		{
			// OffScreenPass = Ry::RendAPI->CreateRenderPass();
			// OffScreenPass->SetFramebufferDescription(OffScreenDesc);
			//
			// // Add attachments to main sub-pass
			// int32 MainPass = OffScreenPass->CreateSubpass();
			// OffScreenPass->AddSubpassAttachment(MainPass, MainColor);
			// OffScreenPass->AddSubpassAttachment(MainPass, MainDepth);
			// OffScreenPass->AddSubpassAttachment(MainPass, MainStencil);
			// OffScreenPass->CreateRenderPass();			
		}

		// Create frame buffer for render pass
		//OffScreenFbo = Ry::RendAPI->CreateFrameBuffer(100, 100, OffScreenDesc);

		//ImposeSceneTextureResource = Ry::RendAPI->CreateResourceSet();

		//OffScreen = new Batch(OffScreenPass, 600, 600);

		// Create a full-texture rect
		SharedPtr<BatchItem> Rect = MakeItem();
		BatchRectangle(Rect, BLUE, 0.0f, 0.0f, 100.0f, 100.0f, 0.0f);
		//OffScreen->AddItem(Rect, "Shape", PipelineState{}, nullptr, 0);

		// Immediately update and render
		//OffScreen->Update();
		//OffScreen->Render();

		// Record the off-screen cmd

		// Render the off-screen image
		//OffScreenCmd->Submit();

		//Bat->SetProjection(Ry::ortho4(0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight(), 0, -1.0f, -1.0f));

		TestWorld = new World2D(Parent);

		SharedPtr<TestEntity> NewEnt = MakeShared(new TestEntity(TestWorld));
		TestWorld->AddEntity(NewEnt);

		TestWorld->AddCustomBatch(Bat);

		Ry::Shader* ImposeShader = Ry::GetOrCompileShader("Impose", "Vertex/Impose", "Fragment/Impose");
		PipelineCreateInfo CreateInfo;
		CreateInfo.RenderPass = MainPass;
		CreateInfo.Blend.bEnabled = false;
		CreateInfo.Depth.bEnableDepthTest = false;
		CreateInfo.bEnableScissorTest = true;
		CreateInfo.PipelineShader = ImposeShader;
		CreateInfo.ViewportWidth = ParentSC->GetSwapChainWidth();
		CreateInfo.ViewportHeight= ParentSC->GetSwapChainHeight();
		ImposePipeline = Ry::RendAPI->CreatePipelineFromShader(CreateInfo, ImposeShader);
		ImposePipeline->CreatePipeline();

		Ry::ResourceSet* ImposeResource = Ry::RendAPI->CreateResourceSet(ImposeShader->GetFragmentReflectionData().GetResources()[0], ParentSC);
		ImposeResource->BindFrameBufferAttachment("SceneTexture", OffScreenBuffer, 0);
		ImposeResource->CreateBuffer();
		ImposeResources.Add(ImposeResource);

		ScreenMesh = new Ry::Mesh(ImposeShader->GetVertexFormat());
		ScreenMesh->GetMeshData()->AddVertex(
		{
				-1.0, -1.0, 0.0,
				0.0, 1.0,
				0.0, 0.0, 0.0
		});
		ScreenMesh->GetMeshData()->AddVertex(
			{
					-1.0, 1.0, 0.0,
					0.0, 0.0,
					0.0, 0.0, 0.0
		});
		ScreenMesh->GetMeshData()->AddVertex(
			{
					1.0, 1.0, 0.0,
					1.0, 0.0,
					0.0, 0.0, 0.0
		});
		ScreenMesh->GetMeshData()->AddVertex(
			{
					1.0, -1.0, 0.0,
					1.0, 1.0,
					0.0, 0.0, 0.0
		});
		ScreenMesh->GetMeshData()->AddTriangle(0, 1, 2);
		ScreenMesh->GetMeshData()->AddTriangle(2, 3, 0);
		ScreenMesh->Update();

		//Bat->SetLayerScissor(6, RectScissor{ 0, 0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight() });
		//Bat->SetLayerScissor(7, RectScissor{ 0, 0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight() });

		// Create UI
		UI = new UserInterface(Bat, GetStyle("Editor"));

		Ry::SharedPtr<Ry::BorderWidget> Root;
		Ry::SharedPtr<Ry::ContentBrowserWidget> BrowserWidget;

		// Build editor UI
		Root = LoadWidget<Ry::BorderWidget>("/Engine/UI/EditorMain.ui");
		if(Root.IsValid())
		{
			BrowserWidget = Root->FindChildWidget<Ry::ContentBrowserWidget>("BrowserWidget");
		}

		/*NewWidgetAssign(Root, BorderWidget)
		.HorAlign(HAlign::CENTER)
		.VertAlign(VAlign::BOTTOM)
		.FillX(1.0f)
		.FillY(1.0f)
		[
			NewWidget(Ry::Splitter)
			.BarThickness(5.0f)
			.Type(SplitterType::VERTICAL)

			+ Ry::Splitter::MakeSlot()
			[
				NewWidget(Ry::BorderWidget)
				//.DefaultBox(Default)
				//.HoveredBox(Hovered)
				.Padding(10.0f)
				.FillX(1.0f)
				[
					NewWidgetAssign(BrowserWidget, Ry::ContentBrowserWidget)
				]
			]

			+ Ry::Splitter::MakeSlot()
			[
				NewWidget(Ry::Splitter)
				.BarThickness(4.0f)
				.Type(SplitterType::HORIZONTAL)
				
				+ Ry::Splitter::MakeSlot()
				[
					NewWidget(BorderWidget)
					.BoxStyleName("TestIcon1")
		//			.DefaultBox(Default2)
					.Padding(75.0f)
				]
				+ Ry::Splitter::MakeSlot()
				[
					NewWidget(BorderWidget)
					.BoxStyleName("TestIcon2")
//					.DefaultBox(Default3)
					.Padding(75.0f)
				]

			]

		];*/
		
		// Create the content browser utility
		ContentBrowse = new ContentBrowser(BrowserWidget);

		UI->AddRoot(Root);

		// Initialize the directory now that the content browser is added to the root
		ContentBrowse->SetDirectory("/Engine");
	}

	void MainEditorLayer::Update(float Delta)
	{
		TestWorld->Update(Delta);
	}

	void MainEditorLayer::Render()
	{
		Bat->Render();

		RecordCmds();

		Cmd->Submit();

		//TestWorld->Draw();
	}

	bool MainEditorLayer::OnEvent(const Event& Ev)
	{		
		return UI->OnEvent(Ev);
	}

	void MainEditorLayer::OnResize(int32 Width, int32 Height)
	{
		// todo: cant re record primary command buffer here unless we explicitly re-record secondary buffers
		// the below just marks the secondaries for needing recording

		Bat->SetRenderPass(ParentSC->GetDefaultRenderPass());

		Bat->Resize(Width, Height);

		UI->Redraw();

		TestWorld->Resize(Width, Height);
		// ShapeBatch->Render();
		// TextureBatch->Render();
		// TextBatch->Render();
		//
		// RecordCmds();
	}

	void MainEditorLayer::RecordCmds()
	{
		Cmd->Reset();
		
		Cmd->BeginCmd();
		{
			Cmd->BeginRenderPass(OffScreenPass, OffScreenBuffer, true);
			{

				for (int32 Layer = 0; Layer < Bat->GetLayerCount(); Layer++)
				{
					CommandBuffer* BatBuffer = Bat->GetCommandBuffer(Layer);

					if (BatBuffer)
					{
						Cmd->DrawCommandBuffer(BatBuffer);
					}
				}

			}
			Cmd->EndRenderPass();

			Cmd->BeginRenderPass(MainPass, OnScreenBuffer, false);
			{
				Cmd->BindPipeline(ImposePipeline);

				Cmd->SetViewportSize(0, 0, ParentSC->GetSwapChainWidth(), ParentSC->GetSwapChainHeight());
				Cmd->SetScissorSize(0, 0, ParentSC->GetSwapChainWidth(), ParentSC->GetSwapChainHeight());

				Cmd->BindResources(ImposeResources.GetData(), ImposeResources.GetSize());

				Cmd->DrawVertexArrayIndexed(ScreenMesh->GetVertexArray(), 0, ScreenMesh->GetMeshData()->GetIndexCount());
			}
			Cmd->EndRenderPass();
		}
		Cmd->EndCmd();
	}
	
}
