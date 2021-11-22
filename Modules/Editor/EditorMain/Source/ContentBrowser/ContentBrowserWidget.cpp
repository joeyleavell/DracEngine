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
	ContentBrowserItem::ContentBrowserItem(Ry::String TileIconStyle, Ry::String Name)
	{
		this->TileIconStyle = TileIconStyle;
		this->Name = Name;
	}

	ContentBrowserItem::~ContentBrowserItem()
	{
		//std::cout << "killing " << *Lab->GetText() << std::endl;
	}

	void ContentBrowserItem::Construct()
	{
		SetChild(
			NewWidget(Ry::VerticalPanel)
			+
			VerticalPanel::MakeSlot()
			[
				NewWidgetAssign(Icon, Ry::BorderWidget)
				.BoxStyleName(TileIconStyle)
//				.DefaultImage(Tex)
	//			.HoveredImage(Tex)
//				.HoveredImageTint(WHITE.ScaleRGB(0.5f))
				.Padding(30.0f)
			]

			+
			VerticalPanel::MakeSlot()
			[
				NewWidgetAssign(Lab, Ry::Label)
				.TextStyleName("Normal")
				.Text(Name)
			]
		);
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
							.BoxStyleName("UpArrowIcon")
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
		Ry::SharedPtr<ContentBrowserItem> NewItem = MakeShared(new ContentBrowserItem("DirectoryIcon", Name));
		Ry::SharedPtr<Widget> AsWidget = CastShared<Widget>(NewItem);
		Grid->AppendSlot(AsWidget);

		NewItem->Construct();

		return NewItem;
	}

	Ry::SharedPtr<ContentBrowserItem> ContentBrowserWidget::AddFile(Ry::String Name)
	{
		Ry::SharedPtr<ContentBrowserItem> NewItem = MakeShared(new ContentBrowserItem("FileIcon", Name));
		Ry::SharedPtr<Widget> AsWidget = CastShared<Widget>(NewItem);

		Grid->AppendSlot(AsWidget);

		NewItem->Construct();

		return NewItem;
	}

	void ContentBrowserWidget::ClearChildren()
	{
		Grid->ClearChildren();
	}
	
}
