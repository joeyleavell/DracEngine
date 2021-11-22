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
#include "WidgetManager.h"

namespace Ry
{
	ContentBrowserItem::ContentBrowserItem(Ry::String TileIconStyle, Ry::String Name)
	{
		this->TileIconStyle = TileIconStyle;
		this->Name = Name;

		Ry::SharedPtr<Widget> ItemWidget = LoadWidget<Ry::Widget>("/Engine/UI/ContentBrowserItem.ui");

		// Set the style of the icon
		Icon = ItemWidget->FindChildWidget<Ry::BorderWidget>("Icon");
		Icon->BoxStyleName = TileIconStyle;

		// Set the name of the tile
		Lab = ItemWidget->FindChildWidget<Ry::Label>("Name");
		Lab->SetText(Name);

		SetChild(ItemWidget);

	}

	ContentBrowserItem::~ContentBrowserItem()
	{
	}

	void ContentBrowserItem::Construct()
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
		Ry::SharedPtr<Widget> ContentBrowser = LoadWidget<Ry::Widget>("/Engine/UI/ContentBrowser.ui");
		if(ContentBrowser.IsValid())
		{
			UpArrow     = ContentBrowser->FindChildWidget<Ry::Button>("UpArrow");
			CurDirLabel = ContentBrowser->FindChildWidget<Ry::TextField>("CurrentDirectory");
			Grid = ContentBrowser->FindChildWidget<Ry::GridPanel>("ContentGrid");
		}

		SetChild(ContentBrowser);
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
