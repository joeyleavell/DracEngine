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
#include "Buttons.h"

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

		// Fill the space available to us
		WidthMode = SIZE_MODE_AUTO;
		HeightMode = SIZE_MODE_AUTO;
//		FillX = 1.0f;
//		FillY = 1.0f;

		SetChild(ItemWidget);

		this->bIsDragging = false;
		this->bIsPressed = false;
		this->bGhostShown = false;
		GhostIconItemSet = MakeItemSet();

	}

	ContentBrowserItem::~ContentBrowserItem()
	{
	}

	bool ContentBrowserItem::OnMouseButtonEvent(const MouseButtonEvent& MouseEv)
	{
		if(IsHovered() && !bIsDragging && MouseEv.ButtonID == MOUSE_BUTTON_LEFT && MouseEv.bPressed)
		{
			Offset.X = (int32) MouseEv.MouseX - Icon->GetAbsolutePosition().X;
			Offset.Y = (int32) MouseEv.MouseY - Icon->GetAbsolutePosition().Y;

			this->bIsPressed = true;
			MarkDirty(this, true);
			return true;
		}

		if(bIsPressed && MouseEv.ButtonID == MOUSE_BUTTON_LEFT && !MouseEv.bPressed)
		{
			this->bIsPressed = false;
			this->bIsDragging = false;

			MarkDirty(this, true);
			return true;
		}

		return false;
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

	bool ContentBrowserItem::OnMouseDragged(const MouseDragEvent& MouseEv)
	{
		if(bIsPressed)
		{
			this->bIsDragging = true;
			LastMouseX = MouseEv.MouseX;
			LastMouseY = MouseEv.MouseY;

			// MarkDirty; this widget needs a re-draw as the widget matches the x,y position of the mouse
			MarkDirty(this, true);

			return true;
		}

		return false;
	}

	void ContentBrowserItem::OnShow(Ry::Batch* Batch)
	{
		SlotWidget::OnShow(Batch);

		if(bIsDragging && !bGhostShown)
		{
			const BoxStyle& IconStyle = Style->GetBoxStyle(Icon->BoxStyleName);

			// Set state of "ghost" icon
			{
				PipelineState State = GetPipelineState(this);
				State.StateID += "_Drag";
				State.Scissor.X = 0;
				State.Scissor.Y = 0;
				State.Scissor.Width = Ry::GetViewportWidth();
				State.Scissor.Height = Ry::GetViewportHeight();

				IconStyle.Default->Show(GhostIconItemSet, Batch, GetWidgetID(), State);

				bGhostShown = true;
			}
		}
	}

	void ContentBrowserItem::OnHide(Ry::Batch* Batch)
	{
		SlotWidget::OnHide(Batch);

		if(bGhostShown)
		{
			const BoxStyle& IconStyle = Style->GetBoxStyle(Icon->BoxStyleName);
			IconStyle.Default->Hide(GhostIconItemSet, Batch);
			bGhostShown = false;
		}
	}

	void ContentBrowserItem::Draw()
	{
		SlotWidget::Draw();

		if(bIsDragging)
		{
			SizeType Size = Icon->ComputeSize();
			const BoxStyle& IconStyle = Style->GetBoxStyle(Icon->BoxStyleName);
			IconStyle.Default->Draw(GhostIconItemSet, LastMouseX - Offset.X, LastMouseY - Offset.Y, Size.Width, Size.Height);
		}
	}

	ContentBrowserWidget::ContentBrowserWidget()
	{
		// Take up entire space available to us
		WidthMode = SIZE_MODE_PERCENTAGE;
		HeightMode = SIZE_MODE_PERCENTAGE;
		FillX = 1.0f;
		FillY = 1.0f;
		VerticalAlign = VERT_BOTTOM_ALIGN;

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

		return NewItem;
	}

	Ry::SharedPtr<ContentBrowserItem> ContentBrowserWidget::AddFile(Ry::String Name)
	{
		Ry::SharedPtr<ContentBrowserItem> NewItem = MakeShared(new ContentBrowserItem("FileIcon", Name));
		Ry::SharedPtr<Widget> AsWidget = CastShared<Widget>(NewItem);

		Grid->AppendSlot(AsWidget);

		return NewItem;
	}

	void ContentBrowserWidget::ClearChildren()
	{
		Grid->ClearChildren();
	}

	bool ContentBrowserWidget::OnPathDrop(const PathDropEvent& PathDropEvent)
	{
		if(IsHovered())
		{
			// Create array list
			Ry::ArrayList<Ry::String> ImportAssets;
			for(int32 AssetIndex = 0; AssetIndex < PathDropEvent.PathCount; AssetIndex++)
			{
				ImportAssets.Add(PathDropEvent.Paths[AssetIndex]);
			}

			ImportAssetsCallback.Broadcast(ImportAssets);
			return true;
		}

		return false;
	}
}
