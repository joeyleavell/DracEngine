#include "ContentBrowser/ContentBrowserWidget.h"
#include "Widget/Layout/VerticalPanel.h"
#include "Widget/BorderWidget.h"
#include "Widget/Label.h"
#include "TextureAsset.h"
#include "Manager/AssetManager.h"
#include "VectorFontAsset.h"
#include "Widget/Layout/HorizontalPanel.h"
#include "Widget/Layout/ScrollPane.h"
#include "Widget/Input/Button.h"

namespace Ry
{
	ContentBrowserItem::ContentBrowserItem(Texture* Texture, BitmapFont* Font, Ry::String Name)
	{
		
		SetChild(
			NewWidget(Ry::VerticalPanel)
			+
			VerticalPanel::MakeSlot()
			[
				NewWidgetAssign(Icon, Ry::BorderWidget)
				.DefaultImage(Texture)
				.HoveredImage(Texture)
				.HoveredImageTint(WHITE.ScaleRGB(0.5f))
				.Padding(30.0f)
			]

			+
				VerticalPanel::MakeSlot()
			[
				NewWidgetAssign(Lab, Ry::Label)
				.Text(Name)
				.Font(Font)
				.Color(WHITE)
			]
		);
	}

	ContentBrowserItem::~ContentBrowserItem()
	{
		//std::cout << "killing " << *Lab->GetText() << std::endl;
	}

	bool ContentBrowserItem::OnMouseClicked(const MouseClickEvent& MouseEv)
	{
		if(MouseEv.bDoubleClick && Icon->IsHovered())
		{
			OnDoubleClick.Broadcast(this);

			return true;
		}

		return false;
	}

	ContentBrowserWidget::ContentBrowserWidget()
	{
		VectorFontAsset* Font = Ry::AssetMan->LoadAsset<VectorFontAsset>("/Engine/Fonts/arial.ttf", "font/truetype");
		TextFont = Font->GenerateBitmapFont(20);

		TextureAsset* UpArrowAsset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/up-arrow.png", "image");
		UpArrowTexture = UpArrowAsset->CreateRuntimeTexture();

		// Create directory grid
		SetChild(
			NewWidget(VerticalPanel)
			+ 
			VerticalPanel::MakeSlot()
			[
				NewWidget(BorderWidget)
				[
					NewWidget(HorizontalPanel)
					+
					HorizontalPanel::MakeSlot()
					.SetBottomPadding(10.0f)
					[
						// Up directory
						NewWidgetAssign(UpArrow, Ry::Button)

						[
							NewWidget(BorderWidget)
							.DefaultImage(UpArrowTexture)
							.DefaultImageTint(WHITE)
							.HoveredImage(UpArrowTexture)
							.HoveredImageTint(WHITE.ScaleRGB(0.8f))
							.PressedImage(UpArrowTexture)
							.PressedImageTint(WHITE.ScaleRGB(0.6f))
							.Padding(10.0f)
						]
					]

					// Current Directory
					+
					HorizontalPanel::MakeSlot()
					.SetBottomPadding(10.0f)
					[
						NewWidgetAssign(CurDirLabel, TextField)
						.Color(WHITE)
						.Font(TextFont)
					]

				]

			]

			+ VerticalPanel::MakeSlot()
			[
				NewWidgetAssign(Pane, ScrollPane)
				.Width(600)
				.Height(300)
				
				+ ScrollPane::MakeSlot()
				[
					NewWidgetAssign(Grid, GridPanel)
					.CellWidth(200.0f)
					.CellHeight(100.0f)
				]

			]
		);

		TextureAsset* Asset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/Icon.png", "image");
		DirectoryTexture = Asset->CreateRuntimeTexture();

		TextureAsset* FileAsset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/file.png", "image");
		FileTexture = FileAsset->CreateRuntimeTexture();

	}

	ContentBrowserWidget::~ContentBrowserWidget()
	{
		
	}

	void ContentBrowserWidget::SetCurrentDirectory(Ry::String Dir)
	{
		CurDirLabel->SetText(Dir);
	}

	Ry::SharedPtr<ContentBrowserItem> ContentBrowserWidget::AddDirectory(Ry::String Name)
	{
		Ry::SharedPtr<ContentBrowserItem> NewItem = MakeShared(new ContentBrowserItem(DirectoryTexture, TextFont, Name));
		Ry::SharedPtr<Widget> AsWidget = CastShared<Widget>(NewItem);
		Grid->AppendSlot(AsWidget);

		return NewItem;
	}

	Ry::SharedPtr<ContentBrowserItem> ContentBrowserWidget::AddFile(Ry::String Name)
	{
		Ry::SharedPtr<ContentBrowserItem> NewItem = MakeShared(new ContentBrowserItem(FileTexture, TextFont, Name));
		Ry::SharedPtr<Widget> AsWidget = CastShared<Widget>(NewItem);

		Grid->AppendSlot(AsWidget);

		return NewItem;
	}

	void ContentBrowserWidget::ClearChildren()
	{
		Grid->ClearChildren();
	}
	
}
