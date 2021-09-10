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
		// Initialize primary command buffer
		Cmd = Ry::RendAPI->CreateCommandBuffer(Parent);

		Shader* UIShader = GetShader("Shape");
		Shader* UIText = GetShader("Font");
		Shader* UITexture = GetShader("Texture");

		ShapeBatch = new Batch(ParentSC, ParentSC->GetDefaultRenderPass(), UIShader, false);
		TextBatch = new Batch(ParentSC, ParentSC->GetDefaultRenderPass(), UIText, true);
		TextureBatch = new Batch(ParentSC, ParentSC->GetDefaultRenderPass(), UITexture, true);

		//TextureBatch->AddItem(TextureItem, Tex);
		//TextureBatch->Update();
		//TextureBatch->Render();

		// Create UI
		UI = new UserInterface;
		UI->SetShapeBatch(ShapeBatch);
		UI->SetTextBatch(TextBatch);
		UI->SetTextureBatch(TextureBatch);

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
		ShapeBatch->Render();
		TextureBatch->Render();
		TextBatch->Render();

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

		ShapeBatch->SetRenderPass(ParentSC->GetDefaultRenderPass());
		TextureBatch->SetRenderPass(ParentSC->GetDefaultRenderPass());
		TextBatch->SetRenderPass(ParentSC->GetDefaultRenderPass());

		ShapeBatch->Resize(Width, Height);
		TextureBatch->Resize(Width, Height);
		TextBatch->Resize(Width, Height);

		UI->Draw();

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
			Cmd->BeginRenderPass(ParentSC->GetDefaultRenderPass());
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
	
}
