#include "ContentBrowser/ContentBrowserWidget.h"
#include "Widget/VerticalPanel.h"
#include "Widget/BorderWidget.h"
#include "Widget/Label.h"
#include "TextureAsset.h"
#include "Manager/AssetManager.h"
#include "VectorFontAsset.h"

namespace Ry
{

	ContentBrowserWidget::ContentBrowserWidget()
	{
		// Create directory grid
		SetChild(
			NewWidgetAssign(Grid, GridLayout)
			.SetCellWidth(200.0f)
			.SetCellHeight(100.0f)
		);

		VectorFontAsset* Font = Ry::AssetMan->LoadAsset<VectorFontAsset>("/Engine/Fonts/arial.ttf", "font/truetype");
		TextFont = Font->GenerateBitmapFont(20);

		TextureAsset* Asset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/Icon.png", "image");
		DirectoryTexture = Asset->CreateRuntimeTexture();

		TextureAsset* FileAsset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/file.png", "image");
		FileTexture = FileAsset->CreateRuntimeTexture();
		
	}

	ContentBrowserWidget::~ContentBrowserWidget()
	{
		
	}

	void ContentBrowserWidget::AddDirectory(Ry::String Name)
	{
		Grid->AppendSlot(
			NewWidget(Ry::VerticalLayout)
			+
			NewWidget(Ry::BorderWidget)
			.DefaultImage(DirectoryTexture)
			.HoveredImage(DirectoryTexture, WHITE.ScaleRGB(0.5f))
			.Padding(30.0f, 30.0f)
			+
			NewWidget(Ry::Label)
			.SetText(Name)
			.SetStyle(TextFont, WHITE)
		);

	}

	void ContentBrowserWidget::AddFile(Ry::String Name)
	{
		Grid->AppendSlot(
			NewWidget(Ry::VerticalLayout)
			+
			NewWidget(Ry::BorderWidget)
			.DefaultImage(FileTexture)
			.HoveredImage(FileTexture, WHITE.ScaleRGB(0.5f))
			.Padding(30.0f, 30.0f)
			+
			NewWidget(Ry::Label)
			.SetText(Name)
			.SetStyle(TextFont, WHITE)
		);
	}
	
}
