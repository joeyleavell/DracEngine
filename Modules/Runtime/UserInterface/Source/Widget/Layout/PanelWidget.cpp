#include "Widget/Layout/PanelWidget.h"
#include "Core/Globals.h"

namespace Ry
{
	
	PanelWidget::PanelWidget()
	{

	}

	SharedPtr<Widget> PanelWidget::FindChildWidgetById(const Ry::String& Id) const
	{
		for (SharedPtr<Widget> Child : Children)
		{
			if(Child->GetId() == Id)
			{
				return Child;
			}
			else
			{
				SharedPtr<Widget> ChildWidget = Child->FindChildWidgetById(Id);
				if(ChildWidget.IsValid())
				{
					return ChildWidget;
				}
			}
		}

		return SharedPtr<Widget>();
	}

	void PanelWidget::SetStyle(const Ry::StyleSet* Style)
	{
		Widget::SetStyle(Style);

		for(SharedPtr<Widget> Child : Children)
		{
			Child->SetStyle(Style);
		}
	}

	SharedPtr<PanelWidgetSlot> PanelWidget::AppendSlot(SharedPtr<Widget> Widget)
	{
		Children.Add(Widget);

		// Set the widget's parent
		Widget->SetStyle(Style);
		Widget->SetParent(this);
		Widget->SetVisible(IsVisible(), true); // Child matches our visibility
		
		return SharedPtr<PanelWidgetSlot>();
	}

	void PanelWidget::OnShow(Ry::Batch* Batch)
	{
	}

	void PanelWidget::OnHide(Ry::Batch* Batch)
	{
	}

	void PanelWidget::SetParent(Widget* Parent)
	{
		Widget::SetParent(Parent);

		// Update child's depth
		for (SharedPtr<Widget> Child : Children)
		{
			Child->SetParent(this);
		}
	}

	void PanelWidget::GetAllChildren(Ry::ArrayList<Widget*>& OutChildren)
	{
		for (SharedPtr<Widget> Child : Children)
		{
			if (Child.IsValid())
			{
				OutChildren.Add(Child.Get());
				Child->GetAllChildren(OutChildren);
			}
		}

	}

	void PanelWidget::Draw()
	{
		// Get clip space for entire widget
		RectScissor ClipSpace = GetClipSpace(nullptr);

		bool bUpdateBatch = false;

		for (SharedPtr<Widget> Child : Children)
		{
			Point Pos = Child->GetAbsolutePosition();
			SizeType ContentSize = Child->ComputeSize();

			RectScissor WidgetBounds{ Pos.X, Pos.Y, ContentSize.Width, ContentSize.Height };

			// Do cull test
			if (!ClipSpace.IsEnabled() || ClipSpace.TestAgainst(WidgetBounds))
			{
				if (!Child->IsVisible())
				{
					Child->SetVisible(true, true);
					bUpdateBatch = true;
				}

				Child->Draw();
			}
			else
			{
				
				if (Child->IsVisible())
				{
					Child->SetVisible(false, true);
					bUpdateBatch = true;
				}
			}

		}

		if (bUpdateBatch)
		{
			//MarkDirty(this);
		}

	}

	PanelWidget& PanelWidget::operator+(Ry::SharedPtr<Widget>& Widget)
	{
		AppendSlot(Widget);

		return *this;
	}

	void PanelWidget::SetVisibleInternal(bool bVisibility, bool bPropagate)
	{
		Widget::SetVisibleInternal(bVisibility, bPropagate);

		if (bPropagate)
		{
			for (SharedPtr<Widget> Child : Children)
			{
				// Set child to its own visibility, needs to refresh
				Child->SetVisibleInternal(bVisibility, true);
			}
		}
	}

	bool PanelWidget::OnMouseEvent(const MouseEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseEvent(MouseEv);

		// This means we're hovering, pass to children
		for (SharedPtr<Widget> Child : Children)
		{
			Child->OnMouseEvent(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnMouseButtonEvent(const MouseButtonEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseButtonEvent(MouseEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnMouseButtonEvent(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnMouseClicked(const MouseClickEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseClicked(MouseEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnMouseClicked(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnMouseDragged(const MouseDragEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseDragged(MouseEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnMouseDragged(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnMouseScroll(const MouseScrollEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseScroll(MouseEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnMouseScroll(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnKey(const KeyEvent& KeyEv)
	{
		bool bHandled = Widget::OnKey(KeyEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnKey(KeyEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnChar(const CharEvent& CharEv)
	{
		bool bHandled = Widget::OnChar(CharEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnChar(CharEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnPathDrop(const PathDropEvent& PathDropEv)
	{
		bool bHandled = Widget::OnPathDrop(PathDropEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnPathDrop(PathDropEv);
		}

		return bHandled;
	}

	void PanelWidget::ClearChildren()
	{
		for (SharedPtr<Widget> Child : Children)
		{
			Child->SetVisible(false, true);
		}

		Children.Clear();
	}

	// SizeType PanelWidget::GetSlotSize(const Widget* ForWidget, bool bIncludePadding) const
	// {
	// 	//SharedPtr<PanelWidgetSlot> ThisSlot = WidgetSlots.Get(const_cast<Widget* const>(ForWidget));
	// 	SizeType Unnormalized = GetSlotSizeUnnormalized(ForWidget);
	// 	//Unnormalized.Width += ThisSlot->PaddingLeft + ThisSlot->PaddingRight;
	// 	//Unnormalized.Height += ThisSlot->PaddingTop + ThisSlot->PaddingBottom;
	//
	// 	// Find out this slot's proportion of the total and normalize it
	// 	float WidthSum = 0.0f;
	// 	float HeightSum = 0.0f;
	// 	for (SharedPtr<Widget> Wid : Children)
	// 	{
	// 		// Account for the padding
	// 		SharedPtr<PanelWidgetSlot> Slot = WidgetSlots.Get(Wid.Get());
	// 		WidthSum += Slot->PaddingLeft + Slot->PaddingRight;
	// 		HeightSum += Slot->PaddingTop + Slot->PaddingBottom;
	//
	// 		SizeType Size = GetSlotSizeUnnormalized(Wid.Get(), true);
	// 		WidthSum += Size.Width;
	// 		HeightSum += Size.Height;
	// 	}
	// 	
	// 	// Check if a dimension has exceeded the max size. If it has, start squeezing down elements.
	// 	/**/
	//
	// 	if (HeightSum > ThisSize.Height)
	// 	{
	// 		float Fraction = Unnormalized.Height / (float)HeightSum;
	// 		float NewHeight = ThisSize.Height * Fraction;
	// 		Unnormalized.Height = (int32)NewHeight;
	// 	}
	//
	// 	return Unnormalized;
	//
	// }

	SizeType PanelWidget::GetUnscaledSlotSize(const Widget* ForWidget) const
	{
		if (WidgetSlots.Contains(const_cast<Widget* const>(ForWidget)))
		{
			SharedPtr<PanelWidgetSlot> Slot = WidgetSlots.Get(const_cast<Widget* const>(ForWidget));
			SizeType Size;

			if (ForWidget)
			{
				if (Slot->bSizeToContent)
				{
					// This overrides every other size setting
					Size = ForWidget->ComputeSize();
				}
				else
				{
					// First check 
					int32 Width;
					int32 Height;
					SizeType ThisSize;

					// GetScaledSlotSize will tell us the real size provided to us post scaling
					if (Slot->HorizontalAlignMode == HOR_ALIGN_FILL || Slot->VerticalAlignMode == VERT_ALIGN_FILL)
						ThisSize = Widget::GetScaledSlotSize(this);

					if (Slot->HorizontalAlignMode == HOR_ALIGN_FILL)
						Width = ThisSize.Width;
					else
						Width = Slot->SlotWidth;

					if (Slot->VerticalAlignMode == VERT_ALIGN_FILL)
						Height = ThisSize.Height;
					else
						Height = Slot->SlotHeight;

					Size = SizeType{ (int32)Width, (int32)Height };
				}
			}

			return Size;
		}
		else
		{
			Ry::Log->LogError("Tried to call PanelWidget::GetSlotSize on a widget that wasn't a child");
		}

		return SizeType{ 0, 0 };
	}

	SizeType PanelWidget::GetUnscaledOccupiedSize(const Widget* ForWidget) const
	{
		SharedPtr<PanelWidgetSlot> Slot = WidgetSlots.Get(const_cast<Widget* const>(ForWidget));
		SizeType UnscaledSlotSize = GetUnscaledSlotSize(ForWidget);
		UnscaledSlotSize.Width += Slot->PaddingLeft + Slot->PaddingRight;
		UnscaledSlotSize.Height += Slot->PaddingBottom + Slot->PaddingTop;

		return UnscaledSlotSize;
	}

	SizeType PanelWidget::GetScaledOccupiedSize(const Widget* ForWidget) const
	{
		// Padding is guaranteed
		SharedPtr<PanelWidgetSlot> Slot = WidgetSlots.Get(const_cast<Widget* const>(ForWidget));
		SizeType ScaledSlotSize = GetScaledSlotSize(ForWidget);
		ScaledSlotSize.Width += Slot->PaddingLeft + Slot->PaddingRight;
		ScaledSlotSize.Height += Slot->PaddingBottom + Slot->PaddingTop;

		return ScaledSlotSize;
	}

	/*int32 PanelWidget::NormalizeWidth(int32 InWidth)
	{
		SizeType ThisSize = Widget::GetSlotSize(this);
		float WidthSum = 0.0f;
		
		for (SharedPtr<Widget> Wid : Children)
		{
			// Account for the padding
			SharedPtr<PanelWidgetSlot> Slot = WidgetSlots.Get(Wid.Get());
			WidthSum += Slot->PaddingLeft + Slot->PaddingRight;

			SizeType Size = GetSlotSizeUnnormalized(Wid.Get(), true);
			WidthSum += Size.Width;
		}

		if (WidthSum > ThisSize.Width)
		{
			float Fraction = / (float)WidthSum;
			float NewWidth = ThisSize.Width * Fraction;
			Unnormalized.Width = (int32)NewWidth;
		}
	}

	int32 PanelWidget::NormalizeHeight(int32 InHeight)
	{
	}*/


	/*RectScissor PanelWidget::GetClipSpace(const Widget* ForWidget) const
	{
		if(!ForWidget)
		{
			Point Abs = GetAbsolutePosition();
			SizeType Size = ComputeSize();
			return RectScissor{ Abs.X, Abs.Y, Size.Width, Size.Height };
		}
		
		SharedPtr<PanelWidgetSlot> Slot = WidgetSlots.Get(const_cast<Widget* const>(ForWidget));
		if(Slot.IsValid())
		{
			Point Abs = Slot->GetWidget()->GetAbsolutePosition();
			if(Slot->bSizeToContent)
			{
				// Return size of the content
				SizeType ContentSize = Slot->GetWidget()->ComputeSize();
				return RectScissor{ Abs.X, Abs.Y, ContentSize.Width, ContentSize.Height};
			}
			else
			{
				// Return specified size of the slot
				return RectScissor{ Abs.X, Abs.Y, (int32)Slot->SlotWidth, (int32)Slot->SlotHeight };
			}
		}

		Ry::Log->LogError("Slot did not exist for widget");

		return RectScissor{ 0, 0, 0, 0 };
	}*/
}
