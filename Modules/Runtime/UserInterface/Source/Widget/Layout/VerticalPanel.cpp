#include "Widget/Layout/VerticalPanel.h"

namespace Ry
{

	VerticalPanelSlot  VerticalPanel::MakeSlot()
	{
		VerticalPanelSlot NewSlot;
		return NewSlot;
	}

	Ry::SharedPtr<PanelWidgetSlot> VerticalPanel::AppendSlot(Ry::SharedPtr<Ry::Widget> Widget)
	{
		PanelWidget::AppendSlot(Widget);

		// Create widget
		SharedPtr<VerticalPanelSlot > PanelSlot = MakeShared(new VerticalPanelSlot(Widget));
		ChildrenSlots.Add(PanelSlot);

		WidgetSlots.Insert(Widget.Get(), PanelSlot);

		return PanelSlot;
	}

	/**
	 * Arrange widgets vertically.
	 */
	void VerticalPanel::Arrange()
	{
		// Default margin: 5px
		int32 CurrentY = static_cast<int32>(0);

		for (int32 VBoxIndex = 0; VBoxIndex < ChildrenSlots.GetSize(); VBoxIndex++)
		{
			SharedPtr<VerticalPanelSlot > ChildSlot = ChildrenSlots[ChildrenSlots.GetSize() - VBoxIndex - 1];
			SharedPtr<Ry::Widget> Widget = ChildSlot->GetWidget();
			SizeType ContentSize = GetScaledSlotSize(Widget.Get());
			// if(ChildSlot->bSizeToContent)
			// {
			// 	ContentSize = Widget->ComputeSize();
			// }
			// else
			// {
			// 	ContentSize = SizeType{ (int32) ChildSlot->SlotWidth, (int32)ChildSlot->SlotHeight };
			// }

			CurrentY += static_cast<int32>(ChildSlot->PaddingBottom);

			// Set the widget's relative position
			Widget->SetRelativePosition(static_cast<float>(ChildSlot->PaddingLeft), static_cast<float>(CurrentY));
			Widget->Arrange();

			CurrentY += static_cast<int32>(ContentSize.Height + ChildSlot->PaddingTop);

		}
	}

	SizeType VerticalPanel::ComputeSize() const
	{
		SizeType Result;
		Result.Width = 0;
		Result.Height = 0;

		// Width is 2 * Margin + MaxChildWidth

		if (!ChildrenSlots.IsEmpty())
		{

			int32 MaxChildWidth = 0;

			for (SharedPtr<VerticalPanelSlot > ChildSlot : ChildrenSlots)
			{
				SizeType SlotSize = ChildSlot->GetWidget()->ComputeSize();// GetSlotSize(ChildSlot->GetWidget().Get());

				// Add horizontal padding
				SlotSize.Width += (int32)(ChildSlot->PaddingLeft + ChildSlot->PaddingRight);

				// Check if max width
				if (SlotSize.Width > MaxChildWidth)
				{
					MaxChildWidth = SlotSize.Width;
				}

				// Calculate height
				Result.Height += static_cast<int32>(ChildSlot->PaddingTop);
				Result.Height += static_cast<int32>(SlotSize.Height);
				Result.Height += static_cast<int32>(ChildSlot->PaddingBottom);
			}

			Result.Width += MaxChildWidth;
		}

		return Result;

		//return Widget::GetSlotSize(this);
	}

	void VerticalPanel::ClearChildren()
	{
		PanelWidget::ClearChildren();

		ChildrenSlots.Clear();
	}

	SizeType VerticalPanel::GetScaledSlotSize(const Widget* ForWidget) const
	{
		SharedPtr<PanelWidgetSlot> Slot = WidgetSlots.Get(const_cast<Widget* const>(ForWidget));
		SizeType UnscaledOccupied = GetUnscaledOccupiedSize(ForWidget);
		SizeType ThisSize = Widget::GetScaledSlotSize(this);

		// Find out if the unscaled occupied (including padding) sizes can fit within our slot
		float UnscaledHeightSum = 0.0f;
		for (SharedPtr<Widget> Wid : Children)
		{
			SizeType Size = GetUnscaledOccupiedSize(Wid.Get());
			UnscaledHeightSum += Size.Height;
		}

		// Squeeze down height if needed
		if (UnscaledHeightSum > ThisSize.Height)
		{
			float Fraction = UnscaledOccupied.Height / (float)UnscaledHeightSum;
			float NewHeight = ThisSize.Height * Fraction;
			UnscaledOccupied.Height = (int32)NewHeight;
		}

		// Squeeze down width if necessary
		//float Width = GetSlotSizeUnnormalized(const_cast<Widget* const>(ForWidget), true).Width;
		if (UnscaledOccupied.Width > ThisSize.Width)
		{
			UnscaledOccupied.Width = ThisSize.Width;
		}

		return UnscaledOccupied;
	}

}
