#include "Widget/Layout/ScrollPane.h"

namespace Ry
{
	ScrollPane::ScrollPane()
	{
		this->bAllowHorizontalScroll = true;

		this->VerticalScrollAmount = 1.0f;
		this->HorizontalScrollAmount = 0.0f;
		this->ScrollBarThickness = 20.0f;
		this->bVerticalBarPressed = false;
		this->bHorizontalBarPressed = false;
		
		VerticalScrollBar = MakeShared(new BoxDrawable);
		VerticalScrollBar->SetBackgroundColor(WHITE.ScaleRGB(0.4f));
		VerticalScrollBar->SetBorderRadius(0);
		VerticalScrollBar->SetBorderSize(0);

		HorizontalScrollBar = MakeShared(new BoxDrawable);
		HorizontalScrollBar->SetBackgroundColor(WHITE.ScaleRGB(0.4f));
		HorizontalScrollBar->SetBorderRadius(0);
		HorizontalScrollBar->SetBorderSize(0);
		
		DebugRect = Ry::MakeItem();

		HorizontalBarItem = MakeItemSet();
		VerticalBarItem = MakeItemSet();

		VerticalPlacement = SCROLL_METHOD_TOP_TO_BOTTOM;
	}

	float ScrollPane::GetVerticalScrollAmount()
	{
		return VerticalScrollAmount;
	}

	float ScrollPane::GetHorizontalScrollAmount()
	{
		return HorizontalScrollAmount;
	}

	void ScrollPane::VerticalScroll(float Pixels)
	{
		SizeType ThisSize = ComputeSize();
		
		SizeType ChildrenSize = ComputeChildrenSize();
		float HiddenAmountY = (float)(ChildrenSize.Height - ThisSize.Height);

		SetVerticalScrollAmount(VerticalScrollAmount + Pixels / HiddenAmountY);
	}

	void ScrollPane::HorizontalScroll(float Pixels)
	{
		SizeType ThisSize = ComputeSize();

		SizeType ChildrenSize = ComputeChildrenSize();
		float HiddenAmountX = (float)(ChildrenSize.Width - ThisSize.Width);

		SetHorizontalScrollAmount(HorizontalScrollAmount + Pixels / HiddenAmountX);
	}

	void ScrollPane::SetHorizontalScrollAmount(float Scroll)
	{
		float Prev = this->HorizontalScrollAmount;
		this->HorizontalScrollAmount = Scroll;

		if (HorizontalScrollAmount > 1.0f)
			HorizontalScrollAmount = 1.0f;
		else if (HorizontalScrollAmount < 0.0f)
			HorizontalScrollAmount = 0.0f;

		if (std::abs(Prev - Scroll) >= 0.000001f)
		{
			// Scroll amount has changed, mark widget as dirty	
			MarkDirty(this);
		}
	}

	void ScrollPane::SetVerticalScrollAmount(float Scroll)
	{
		float Prev = this->VerticalScrollAmount;
		this->VerticalScrollAmount = Scroll;

		if (VerticalScrollAmount > 1.0f)
			VerticalScrollAmount = 1.0f;
		else if (VerticalScrollAmount < 0.0f)
			VerticalScrollAmount = 0.0f;

		if (std::abs(Prev - Scroll) >= 0.000001f)
		{
			// Scroll amount has changed, mark widget as dirty	
			MarkDirty(this);
		}
	}

	ScrollPane::Slot ScrollPane::MakeSlot()
	{
		ScrollPane::Slot NewSlot;
		return NewSlot;
	}

	SharedPtr<PanelWidgetSlot> ScrollPane::AppendSlot(SharedPtr<Widget> Widget)
	{
		PanelWidget::AppendSlot(Widget);

		// Create slot
		SharedPtr<Slot> ScrollSlot = MakeShared(new Slot(Widget));
		ChildrenSlots.Add(ScrollSlot);

		WidgetSlots.Insert(Widget.Get(), ScrollSlot);

		return ScrollSlot;
	}

	void ScrollPane::Draw()
	{
		SizeType ThisSize = ComputeSize();

		Point Abs = GetAbsolutePosition();
		Ry::BatchHollowRectangle(DebugRect, WHITE, Abs.X + 2.0f, Abs.Y + 2.0f, ThisSize.Width - 4.0f, ThisSize.Height - 4.0f, 2.0f, 0.0f);

		Point HorLoc;
		Point VertLoc;
		SizeType HorSize;
		SizeType VertSize;

		GetVertScrollBarBounds(VertLoc, VertSize);
		GetHorScrollBarBounds(HorLoc, HorSize);

		VerticalScrollBar->Draw(VerticalBarItem, (float)VertLoc.X, (float)VertLoc.Y, (float)VertSize.Width, (float)VertSize.Height);
		HorizontalScrollBar->Draw(HorizontalBarItem, (float)HorLoc.X, (float)HorLoc.Y, (float)HorSize.Width, (float)HorSize.Height);

		Ry::PanelWidget::Draw();
	}

	/**
	 * Arrange widgets vertically.
	 */
	void ScrollPane::Arrange()
	{
		// Check if any widget's clip space has changed
		for (SharedPtr<Slot> Child : ChildrenSlots)
		{
			RectScissor Clip = GetClipSpace(Child->GetWidget().Get());

			if (!(Clip == LastClip))
			{
				LastClip = Clip;
				//MarkDirty(this);

				// Already re-rendered everything, don't need to repeat
				//break;
			}
		}

		// The physical size the children occupy
		SizeType ChildrenSize = ComputeChildrenSize();

		// The max space available to us that will fit on the screen
		SizeType ThisSize = Widget::GetScaledSlotSize(this);

		float HiddenAmountX = (float)(ChildrenSize.Width - ThisSize.Width);
		float HiddenAmountY = (float)(ChildrenSize.Height - ThisSize.Height);
		int32 OffsetX = (int32)(HiddenAmountX * HorizontalScrollAmount);
		int32 OffsetY = (int32)(HiddenAmountY * VerticalScrollAmount);
		if (VerticalPlacement == SCROLL_METHOD_TOP_TO_BOTTOM)
		{
			OffsetY = (int32)(HiddenAmountY * (1.0f - VerticalScrollAmount));
		}

		if (OffsetX < 0)
			OffsetX = 0;
		if (OffsetY < 0)
			OffsetY = 0;

		int32 CurrentY = static_cast<int32>(0);

		if(VerticalPlacement == SCROLL_METHOD_TOP_TO_BOTTOM)
		{
			CurrentY = ThisSize.Height;
		}

		for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
		{
			SharedPtr<Ry::Widget> Widget = ChildSlot->GetWidget();
			SizeType ContentSize = Widget->ComputeSize();

			// Add top padding prior to setting relative position
			if(VerticalPlacement == SCROLL_METHOD_BOTTOM_TO_TOP)
			{
				CurrentY += static_cast<int32>(ChildSlot->PaddingBottom);
			}
			else if(VerticalPlacement == SCROLL_METHOD_TOP_TO_BOTTOM)
			{
				CurrentY -= static_cast<int32>(ChildSlot->PaddingTop) + ContentSize.Height;
			}

			float WidgetX = static_cast<float>(ChildSlot->PaddingLeft - OffsetX);
			float WidgetY = static_cast<float>(CurrentY - OffsetY);
			if (VerticalPlacement == SCROLL_METHOD_TOP_TO_BOTTOM)
			{
				WidgetY = static_cast<float>(CurrentY + OffsetY);
			}

			// Set the widget's relative position
			Widget->SetRelativePosition(WidgetX, WidgetY);
			Widget->Arrange();

			if (VerticalPlacement == SCROLL_METHOD_BOTTOM_TO_TOP)
			{
				CurrentY += static_cast<int32>(ContentSize.Height + ChildSlot->PaddingTop);
			}
			else if (VerticalPlacement == SCROLL_METHOD_TOP_TO_BOTTOM)
			{
				CurrentY -= static_cast<int32>(ChildSlot->PaddingBottom);
			}

		}

	}

	PipelineState ScrollPane::GetPipelineState(const Widget* ForWidget) const
	{
		static Ry::String StateId = Ry::to_string(GetWidgetID());
		PipelineState State;
		State.Scissor = GetClipSpace(ForWidget); // Scroll pane doesn't 
		State.StateID = StateId;

		return State;
	}

	void ScrollPane::GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates)
	{
		OutStates.Add(GetPipelineState(nullptr));
	}

	RectScissor ScrollPane::GetClipSpace(const Widget* ForWidget) const
	{
		//return Widget::GetClipSpace(ForWidget);
		
		Point Position = GetAbsolutePosition();
		SizeType ThisSize = ComputeSize();

		RectScissor Res;
		Res.X = Position.X;
		Res.Y = Position.Y;
		Res.Width = ThisSize.Width;
		Res.Height = ThisSize.Height;

		if (Res.Y < 0)
		{
			Res.Height += Res.Y;
			Res.Y = 0;
		}
		if (Res.X < 0)
		{
			Res.Width += Res.X;
			Res.X = 0;
		}

		if (Res.Width < 0)
			Res.Width = 0;

		if (Res.Height < 0)
			Res.Height = 0;

		return Res.Combine(Widget::GetClipSpace(this));
	}

	void ScrollPane::OnShow(Ry::Batch* Batch)
	{
		Widget::OnShow(Batch);

		//Batch->AddItem(DebugRect, "Shape", GetPipelineState(this));

		// Push scroll bar to last layer
		// todo: come up with better way than this
		RectScissor Scissor;
		if(ShouldShowHorizontalScrollbar())
		{
			HorizontalScrollBar->Show(HorizontalBarItem, Batch, -1, GetPipelineState(this));
		}

		if(ShouldShowVertScrollbar())
		{
			VerticalScrollBar->Show(VerticalBarItem, Batch, -1, GetPipelineState(this));
		}
	}

	void ScrollPane::OnHide(Ry::Batch* Batch)
	{
		Widget::OnHide(Batch);

		//Batch->RemoveItem(DebugRect);

		VerticalScrollBar->Hide(VerticalBarItem, Batch);
		HorizontalScrollBar->Hide(HorizontalBarItem, Batch);
	}

	SizeType ScrollPane::ComputeSize() const
	{
		return Widget::GetScaledSlotSize(this);
		/*		if(Parent)
				{
					return Parent->ComputeSize();
				}
				else
				{
					return SizeType{ Ry::GetViewportWidth(), Ry::GetViewportHeight() };
				}*/
	}

	void ScrollPane::ClearChildren()
	{
		PanelWidget::ClearChildren();

		ChildrenSlots.Clear();
	}

	bool ScrollPane::OnMouseScroll(const MouseScrollEvent& MouseEv)
	{
		bool bChildHandle = PanelWidget::OnMouseScroll(MouseEv);

		if (!bChildHandle && !bVerticalBarPressed && IsHovered())
		{
			float ScrollY = MouseEv.ScrollY;

			VerticalScroll(ScrollY * 30.0f);

			return true;
		}

		return bChildHandle;
	}

	bool ScrollPane::OnMouseButtonEvent(const MouseButtonEvent& MouseEv)
	{
		Point VertLoc;
		SizeType VertSize;
		Point HorLoc;
		SizeType HorSize;

		GetVertScrollBarBounds(VertLoc, VertSize);
		GetHorScrollBarBounds(HorLoc, HorSize);

		bool bVertBoundsTest = MouseEv.MouseX >= VertLoc.X && MouseEv.MouseX < VertLoc.X + VertSize.Width;
		bVertBoundsTest &= MouseEv.MouseY >= VertLoc.Y && MouseEv.MouseY < VertLoc.Y + VertSize.Height;

		bool bHorBoundsTest = MouseEv.MouseX >= HorLoc.X && MouseEv.MouseX < HorLoc.X + HorSize.Width;
		bHorBoundsTest &= MouseEv.MouseY >= HorLoc.Y && MouseEv.MouseY < HorLoc.Y + HorSize.Height;

		if (MouseEv.ButtonID == 0)
		{
			bool bHandled = false;
			// Mouse must be inside scroll bar rect to initiate drag
			if (MouseEv.bPressed)
			{
				if (bVertBoundsTest && !bVerticalBarPressed)
				{
					// Start dragging vertical scroll bar
					bVerticalBarPressed = true;

					// Store the initial delta Y position
					RelativeScrollBarPressed.Y = (int32)(MouseEv.MouseY - VertLoc.Y);

					bHandled = true;
				}
				else if (bHorBoundsTest && !bHorizontalBarPressed)
				{
					// Start dragging vertical scroll bar
					bHorizontalBarPressed = true;

					// Store the initial delta Y position
					RelativeScrollBarPressed.X = (int32)(MouseEv.MouseX - HorLoc.X);

					bHandled = true;
				}
			}

			// Mouse must not continue to stay in scroll bar
			if (!MouseEv.bPressed)
			{
				bVerticalBarPressed = false;
				bHorizontalBarPressed = false;
			}

			if(bHandled)
			{
				return true;
			}
		}

		return PanelWidget::OnMouseButtonEvent(MouseEv);
	}

	bool ScrollPane::OnMouseEvent(const MouseEvent& MouseEv)
	{
		if (bVerticalBarPressed)
		{
			SizeType ThisSize = ComputeSize();

			Point VertAbs;
			SizeType VertSize;
			GetVertScrollBarBounds(VertAbs, VertSize);
			float ScrollRange = (float)(ThisSize.Height - VertSize.Height);

			// get new location
			float CurY = MouseEv.MouseY;
			float Adjusted = CurY - RelativeScrollBarPressed.Y;
			float Relative = Adjusted - GetAbsolutePosition().Y;

			if (Relative < 0)
				Relative = 0;
			if (Relative > ScrollRange)
				Relative = ScrollRange;

			if (ScrollRange > 0.01f)
			{
				SetVerticalScrollAmount(Relative / ScrollRange);
			}

			return true;
		}

		if (bHorizontalBarPressed)
		{
			SizeType ThisSize = ComputeSize();

			Point HorAbs;
			SizeType HorSize;
			GetHorScrollBarBounds(HorAbs, HorSize);
			float ScrollRange = (ThisSize.Width - ScrollBarThickness) - HorSize.Width;

			// get new location
			float CurX = MouseEv.MouseX;
			float Adjusted = CurX - RelativeScrollBarPressed.X;
			float Relative = Adjusted - GetAbsolutePosition().X;

			if (Relative < 0)
				Relative = 0;
			if (Relative > ScrollRange)
				Relative = ScrollRange;

			if (ScrollRange > 0.01f)
			{
				SetHorizontalScrollAmount(Relative / ScrollRange);
			}

			return true;
		}

		return PanelWidget::OnMouseEvent(MouseEv);
	}

	bool ScrollPane::ShouldShowVertScrollbar()
	{
		SizeType ThisSize = ComputeSize();
		SizeType ChildrenSize = ComputeChildrenSize();

		return ChildrenSize.Height > ThisSize.Height;
	}

	bool ScrollPane::ShouldShowHorizontalScrollbar()
	{
		SizeType ThisSize = ComputeSize();
		SizeType ChildrenSize = ComputeChildrenSize();

		return ChildrenSize.Width > ThisSize.Width;
	}

	void ScrollPane::GetVertScrollBarBounds(Point& OutPos, SizeType& OutSize)
	{
		SizeType ThisSize = ComputeSize();

		Point Abs = GetAbsolutePosition();
		SizeType ChildrenSize = ComputeChildrenSize();
		OutSize.Width = (int32)ScrollBarThickness;
		OutSize.Height = (int32)(ThisSize.Height * std::min(ThisSize.Height / (float)ChildrenSize.Height, 1.0f));

		float ScrollRange = (float)(ThisSize.Height - OutSize.Height);
		OutPos.X = (int32)(Abs.X + ThisSize.Width - ScrollBarThickness);
		OutPos.Y = (int32)(Abs.Y + ScrollRange * VerticalScrollAmount);
	}

	void ScrollPane::GetHorScrollBarBounds(Point& OutPos, SizeType& OutSize)
	{
		SizeType ThisSize = ComputeSize();

		Point Abs = GetAbsolutePosition();
		SizeType ChildrenSize = ComputeChildrenSize();
		OutSize.Width = (int32)((ThisSize.Width - ScrollBarThickness) * std::min((ThisSize.Width - ScrollBarThickness) / (float)ChildrenSize.Width, 1.0f));
		OutSize.Height = (int32)(ScrollBarThickness);

		float ScrollRange = (ThisSize.Width - ScrollBarThickness) - OutSize.Width;
		OutPos.X = (int32)(Abs.X + ScrollRange * HorizontalScrollAmount);
		OutPos.Y = (int32)(Abs.Y + ThisSize.Height - ScrollBarThickness);
	}

	// Utility to compute the size of the children embedded within the scroll pane
	SizeType ScrollPane::ComputeChildrenSize() const
	{
		SizeType Result;
		Result.Width = 0;
		Result.Height = 0;

		// Width is 2 * Margin + MaxChildWidth

		if (!ChildrenSlots.IsEmpty())
		{

			int32 MaxChildWidth = 0;

			for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
			{
				SizeType WidgetSize = ChildSlot->GetWidget()->ComputeSize();

				// Add horizontal padding
				WidgetSize.Width += (int32)(ChildSlot->PaddingLeft + ChildSlot->PaddingRight);

				// Check if max width
				if (WidgetSize.Width > MaxChildWidth)
				{
					MaxChildWidth = WidgetSize.Width;
				}

				// Calculate height
				Result.Height += static_cast<int32>(ChildSlot->PaddingTop);
				Result.Height += static_cast<int32>(WidgetSize.Height);
				Result.Height += static_cast<int32>(ChildSlot->PaddingBottom);
			}

			Result.Width += MaxChildWidth;
		}

		return Result;
	}

}