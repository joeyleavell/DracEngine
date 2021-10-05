#include "EditorUI.h"
#include "Interface/RenderAPI.h"
#include "Widget/UserInterface.h"
#include "Widget/BorderWidget.h"
#include "VectorFontAsset.h"
#include "Manager/AssetManager.h"
#include "TextureAsset.h"
#include "Widget/VerticalPanel.h"
#include "Widget/Label.h"
#include "Interface/RenderCommand.h"
#include "ContentBrowser/ContentBrowserWidget.h"

namespace Ry
{
	EditorUI::EditorUI(SwapChain* Parent):
	Layer(Parent)
	{
		TestReflection Refl;
		//std::cout << "Value " << Refl.TestField << std::endl;
		TestRefl* Other = Refl.GetPropertyRef<TestRefl*>("Other2");
		//std::cout << "Value " << Refl.TestField << std::endl;

		std::cout << Other->TestField2 << std::endl;
		
		// Initialize primary command buffer
		Cmd = Ry::RendAPI->CreateCommandBuffer(Parent);
		Bat = new Batch(ParentSC, ParentSC->GetDefaultRenderPass());
		//Bat->SetProjection(Ry::ortho4(0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight(), 0, -1.0f, -1.0f));

		//Bat->SetLayerScissor(6, RectScissor{ 0, 0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight() });
		//Bat->SetLayerScissor(7, RectScissor{ 0, 0, Parent->GetSwapChainWidth(), Parent->GetSwapChainHeight() });

		// Create UI
		UI = new UserInterface(Bat);

		VectorFontAsset* Font = Ry::AssetMan->LoadAsset<VectorFontAsset>("/Engine/Fonts/arial.ttf", "font/truetype");
		BitmapFont* TextFont = Font->GenerateBitmapFont(40);

		Ry::SharedPtr<Ry::BorderWidget> Root;
		Ry::SharedPtr<Ry::ContentBrowserWidget> BrowserWidget;

		BoxDrawable Default;
		Default.SetBackgroundColor(WHITE.ScaleRGB(0.1f));
		Default.SetBorderColor(GREEN);
		Default.SetBorderRadius(10);
		Default.SetBorderSize(0);

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
			NewWidget(Ry::BorderWidget)
			.DefaultBox(Default)
			.HoveredBox(Hovered)
			.Padding(10.0f)
			.FillX(1.0f)
			[
				NewWidgetAssign(BrowserWidget, Ry::ContentBrowserWidget)
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
			Cmd->BeginRenderPass(ParentSC->GetDefaultRenderPass(), false);
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
