#include "Widget/Layout/HorizontalPanel.h"

namespace Ry
{

	HorizontalPanelSlot HorizontalPanel::MakeSlot()
	{
		HorizontalPanelSlot NewSlot;

		return NewSlot;
	}

	/**
	 * Arrange widgets horizontally.
	 */
	void HorizontalPanel::Arrange()
	{
		// Default margin: 5px
		int32 CurrentX = static_cast<int32>(0);

		for (SharedPtr<HorizontalPanelSlot> ChildSlot : ChildrenSlots)
		{
			SharedPtr<Ry::Widget> Widget = ChildSlot->GetWidget();
			SizeType ContentSize = Widget->ComputeSize();

			CurrentX += static_cast<int32>(ChildSlot->PaddingLeft);

			// Set the widget's relative position
			Widget->SetRelativePosition(static_cast<float>(CurrentX), static_cast<float>(ChildSlot->PaddingBottom));
			Widget->Arrange();

			CurrentX += static_cast<int32>(ContentSize.Width + ChildSlot->PaddingRight);

		}
	}

	SizeType HorizontalPanel::ComputeSize() const
	{
		SizeType Result;
		Result.Width = 0;
		Result.Height = 0;

		// Height is 2 * Margin + MaxChildHeight

		if (!ChildrenSlots.IsEmpty())
		{
			int32 MaxChildHeight = 0;
			for (SharedPtr<HorizontalPanelSlot> ChildSlot : ChildrenSlots)
			{
				SizeType WidgetSize = ChildSlot->GetWidget()->ComputeSize();

				// Add vertical padding
				WidgetSize.Height += (int32)(ChildSlot->PaddingTop + ChildSlot->PaddingBottom);

				if (WidgetSize.Height > MaxChildHeight)
				{
					MaxChildHeight = WidgetSize.Height;
				}

				// Calculate width
				Result.Width += static_cast<int32>(ChildSlot->PaddingLeft);
				Result.Width += static_cast<int32>(WidgetSize.Width);
				Result.Width += static_cast<int32>(ChildSlot->PaddingRight);
			}

			Result.Height = MaxChildHeight;
		}

		return Result;
	}

	Ry::SharedPtr<PanelWidgetSlot> HorizontalPanel::AppendSlot(Ry::SharedPtr<Ry::Widget> Widget)
	{
		PanelWidget::AppendSlot(Widget);

		// Create widget
		SharedPtr<HorizontalPanelSlot> PanelSlot = MakeShared(new HorizontalPanelSlot(Widget));
		ChildrenSlots.Add(PanelSlot);

		WidgetSlots.Insert(Widget.Get(), PanelSlot);
		
		return PanelSlot;
	}

	SizeType HorizontalPanel::GetScaledSlotSize(const Widget* ForWidget) const
	{
		SharedPtr<PanelWidgetSlot> Slot = WidgetSlots.Get(const_cast<Widget* const>(ForWidget));
		SizeType UnscaledOccupied = GetUnscaledOccupiedSize(ForWidget);
		SizeType ThisSize = Widget::GetScaledSlotSize(this);

		// Find out if the unscaled occupied (including padding) sizes can fit within our slot
		float UnscaledWidthSum = 0.0f;
		for (SharedPtr<Widget> Wid : Children)
		{
			SizeType Size = GetUnscaledOccupiedSize(Wid.Get());
			UnscaledWidthSum += Size.Width;
		}

		// Squeeze down height if needed
		if (UnscaledWidthSum > ThisSize.Width)
		{
			float Fraction = UnscaledOccupied.Width / (float)UnscaledWidthSum;
			float NewHeight = ThisSize.Width * Fraction;
			UnscaledOccupied.Width = (int32)NewHeight;
		}

		// Squeeze down width if necessary
		//float Width = GetSlotSizeUnnormalized(const_cast<Widget* const>(ForWidget), true).Width;
		if (UnscaledOccupied.Height > ThisSize.Height)
		{
			UnscaledOccupied.Height = ThisSize.Height;
		}

		return UnscaledOccupied;
	}

	void HorizontalPanel::ClearChildren()
	{
		PanelWidget::ClearChildren();

		ChildrenSlots.Clear();
	}


}