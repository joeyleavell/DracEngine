#include "EditorUI.h"
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

namespace Ry
{
	EditorUI::EditorUI(SwapChain* Parent):
	Layer(Parent)
	{
		std::cout << "type name " << *GetType<Ry::ArrayList<unsigned long long int>>()->Name << std::endl;
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
		}

		TestRefl Object; 

		SharedPtr<Ry::Widget> LoadedWid = Ry::LoadWidget("/Engine/UI/TestUI.ui");
		Json AsJson = Ry::Jsonify(*LoadedWid.Get());

		std::cout << "Jsonified: \n" << *AsJson.Stringify() << std::endl;
		
		//TestReflection* NewRefl = GetReflectedClass("Ry::TestReflection")->CreateInstance<TestReflection>();
//		std::cout << "Value " << NewRefl->Other << std::endl;
		//TestRefl* Other = NewRefl->GetPropertyRef<TestRefl*>("Other2");
		//std::cout << "Value " << Refl.TestField << std::endl;
		
		// Initialize primary command buffer
		Cmd = Ry::RendAPI->CreateCommandBuffer(Parent);
		Bat = new Batch(ParentSC, ParentSC->GetDefaultRenderPass());
		//Bat->SetProjection(Ry::ortho4(0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight(), 0, -1.0f, -1.0f));

		//Bat->SetLayerScissor(6, RectScissor{ 0, 0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight() });
		//Bat->SetLayerScissor(7, RectScissor{ 0, 0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight() });

		// Create UI
		UI = new UserInterface(Bat);

		//VectorFontAsset* Font = Ry::AssetMan->LoadAsset<VectorFontAsset>("/Engine/Fonts/arial.ttf", "font/truetype");

		Ry::SharedPtr<Ry::BorderWidget> Root;
		Ry::SharedPtr<Ry::ContentBrowserWidget> BrowserWidget;

		BoxDrawable Default;
		Default.SetBackgroundColor(WHITE.ScaleRGB(0.1f));
		Default.SetBorderColor(GREEN);
		Default.SetBorderRadius(10);
		Default.SetBorderSize(0);

		BoxDrawable Default2;
		Default2.SetBackgroundColor(GREEN);
		Default2.SetBorderColor(GREEN);
		Default2.SetBorderRadius(10);
		Default2.SetBorderSize(0);

		BoxDrawable Default3;
		Default3.SetBackgroundColor(BLUE);
		Default3.SetBorderColor(BLUE);
		Default3.SetBorderRadius(10);
		Default3.SetBorderSize(0);

		BoxDrawable Hovered;
		Hovered.SetBackgroundColor(WHITE.ScaleRGB(0.05f));
		Hovered.SetBorderColor(GREEN);
		Hovered.SetBorderRadius(10);
		Hovered.SetBorderSize(0);

		// Build editor UI
		NewWidgetAssign(Root, BorderWidget)
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
					.DefaultBox(Default2)
					.Padding(75.0f)
				]
				+ Ry::Splitter::MakeSlot()
				[
					NewWidget(BorderWidget)
					.DefaultBox(Default3)
					.Padding(75.0f)
				]

			]

		];
		
		// Create the content browser utility
		ContentBrowse = new ContentBrowser(BrowserWidget);

		UI->AddRoot(Root);
	}

	void EditorUI::Update(float Delta)
	{
	}

	void EditorUI::Render()
	{
		Bat->Render();

		RecordCmds();

		Cmd->Submit();
	}

	bool EditorUI::OnEvent(const Event& Ev)
	{		
		return UI->OnEvent(Ev);
	}

	void EditorUI::OnResize(int32 Width, int32 Height)
	{
		// todo: cant re record primary command buffer here unless we explicitly re-record secondary buffers
		// the below just marks the secondaries for needing recording

		Bat->SetRenderPass(ParentSC->GetDefaultRenderPass());

		Bat->Resize(Width, Height);

		UI->Redraw();
		
		// ShapeBatch->Render();
		// TextureBatch->Render();
		// TextBatch->Render();
		//
		// RecordCmds();
	}

	void EditorUI::RecordCmds()
	{
		Cmd->Reset();
		
		Cmd->BeginCmd();
		{
			Cmd->BeginRenderPass(ParentSC->GetDefaultRenderPass(), true);
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
		}
		Cmd->EndCmd();
	}
	
}
