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

namespace Ry
{
	EditorUI::EditorUI(SwapChain* Parent):
	Layer(Parent)
	{
		// Initialize primary command buffer
		Cmd = Ry::RendAPI->CreateCommandBuffer(Parent);

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


		for (int32 Slot = 0; Slot < 16; Slot++)
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
