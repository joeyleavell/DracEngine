#include "ContentBrowser/ContentBrowserWidget.h"
#include "Widget/VerticalPanel.h"
#include "Widget/BorderWidget.h"
#include "Widget/Label.h"
#include "TextureAsset.h"
#include "Manager/AssetManager.h"
#include "VectorFontAsset.h"
#include "Widget/HorizontalPanel.h"

namespace Ry
{
	ContentBrowserItem::ContentBrowserItem(Texture* Texture, BitmapFont* Font, Ry::String Name)
	{
		SetChild(
			NewWidget(Ry::VerticalLayout)
			+
			NewWidgetAssign(Icon, Ry::BorderWidget)
			.DefaultImage(Texture)
			.HoveredImage(Texture, WHITE.ScaleRGB(0.5f))
			.Padding(30.0f, 30.0f)
			+
			NewWidget(Ry::Label)
			.SetText(Name)
			.SetStyle(Font, WHITE)
		);
	}

	ContentBrowserItem::~ContentBrowserItem()
	{
		
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

		// Create directory grid
		SetChild(
			NewWidget(VerticalLayout)
			.SetMargins(10.0f)
			+NewWidget(BorderWidget)
			[
				NewWidget(HorizontalLayout)
				+
				// Up directory
				NewWidget(BorderWidget)

				// Current Directory
				+
				NewWidgetAssign(CurDirLabel, Label)
				.SetStyle(TextFont, WHITE)
			]

			+ NewWidgetAssign(Grid, GridLayout)
			.SetCellWidth(200.0f)
			.SetCellHeight(100.0f)
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

	ContentBrowserItem* ContentBrowserWidget::AddDirectory(Ry::String Name)
	{
		ContentBrowserItem* NewItem = new ContentBrowserItem(DirectoryTexture, TextFont, Name);		
		Grid->AppendSlot(*NewItem);

		return NewItem;
	}

	ContentBrowserItem* ContentBrowserWidget::AddFile(Ry::String Name)
	{
		ContentBrowserItem* NewItem = new ContentBrowserItem(FileTexture, TextFont, Name);
		Grid->AppendSlot(*NewItem);

		return NewItem;
	}

	void ContentBrowserWidget::ClearChildren()
	{
		Grid->ClearChildren();
	}
	
}
