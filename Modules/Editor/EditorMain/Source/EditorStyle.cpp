#include "EditorStyle.h"
#include "VectorFontAsset.h"
#include "Manager/AssetManager.h"
#include "Style.h"
#include "TextureAsset.h"

namespace Ry
{

	void InitializeEditorStyle()
	{
		// Load editor font
		Ry::VectorFontAsset* Font = Ry::AssetMan->LoadAsset<VectorFontAsset>("/Engine/Fonts/arial.ttf", "font/truetype");
		BitmapFont* NormalTextFont = Font->GenerateBitmapFont(20);

		// Normal text style
		{
			Ry::TextStyle NormalText;
			NormalText.SetColor(WHITE);
			NormalText.SetFont(NormalTextFont);

			EditorStyle.AddTextStyle("Normal", NormalText);
		}

		{
			BoxStyle TestIcon1Style;
			TestIcon1Style.DefaultBox().SetBackgroundColor(GREEN).SetBorderRadius(10);

			EditorStyle.AddBoxStyle("TestIcon1", TestIcon1Style);
		}

		{
			BoxStyle TestIcon2Style;
			TestIcon2Style.DefaultBox().SetBackgroundColor(BLUE).SetBorderRadius(10);

			EditorStyle.AddBoxStyle("TestIcon2", TestIcon2Style);
		}

		// Up arrow style for content browser
		{
			TextureAsset* UpArrowAsset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/up-arrow.png", "image");
			Texture* UpArrowTexture = UpArrowAsset->CreateRuntimeTexture();

			BoxStyle UpIconStyle;
			UpIconStyle.DefaultBoxImage().SetTexture(UpArrowTexture).SetImageTint(WHITE);
			UpIconStyle.HoveredBoxImage().SetTexture(UpArrowTexture).SetImageTint(WHITE.ScaleRGB(0.8f));
			UpIconStyle.PressedBoxImage().SetTexture(UpArrowTexture).SetImageTint(WHITE.ScaleRGB(0.6f));
			EditorStyle.AddBoxStyle("UpArrowIcon", UpIconStyle);
		}

		// Directory icon
		{
			TextureAsset* Asset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/Icon.png", "image");
			Texture* DirectoryTexture = Asset->CreateRuntimeTexture();

			BoxStyle DirectoryIconStyle;
			DirectoryIconStyle.DefaultBoxImage().SetTexture(DirectoryTexture);
			DirectoryIconStyle.HoveredBoxImage().SetTexture(DirectoryTexture).SetImageTint(WHITE.ScaleRGB(0.5f));

			EditorStyle.AddBoxStyle("DirectoryIcon", DirectoryIconStyle);
		}

		// File icon
		{
			TextureAsset* FileAsset = AssetMan->LoadAsset<TextureAsset>("/Engine/Textures/file.png", "image");
			Texture* FileTexture = FileAsset->CreateRuntimeTexture();

			BoxStyle FileIconStyle;
			FileIconStyle.DefaultBoxImage().SetTexture(FileTexture);
			FileIconStyle.HoveredBoxImage().SetTexture(FileTexture).SetImageTint(WHITE.ScaleRGB(0.5f));

			EditorStyle.AddBoxStyle("FileIcon", FileIconStyle);
		}

		// Content browser background
		{
			BoxStyle ContentBrowserBackground;
			ContentBrowserBackground.DefaultBox().SetBackgroundColor(WHITE.ScaleRGB(0.05f));
			EditorStyle.AddBoxStyle("ContentBrowserBackground", ContentBrowserBackground);
		}

		// Background for text boxes
		{
			BoxStyle TextBoxBackground;
			TextBoxBackground.DefaultBox().SetBackgroundColor(WHITE.ScaleRGB(0.2f));
			TextBoxBackground.DefaultBox().SetBorderRadius(5.0f);
			EditorStyle.AddBoxStyle("TextBox", TextBoxBackground);
		}

		// Register the style globally
		RegisterStyle("Editor", &EditorStyle);
	}
	
}

