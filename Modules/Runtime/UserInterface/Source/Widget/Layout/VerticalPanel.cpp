#include "Widget/Layout/VerticalPanel.h"

namespace Ry
{
	void VerticalPanel::Construct(Args& In)
	{
		for (PanelWidget::Slot& Child : In.Slots)
		{
			AppendSlot(Child.GetWidget());
		}
	}

	VerticalPanel::Slot VerticalPanel::MakeSlot()
	{
		VerticalPanel::Slot NewSlot;
		return NewSlot;
	}

	Ry::SharedPtr<PanelWidget::Slot> VerticalPanel::AppendSlot(Ry::SharedPtr<Ry::Widget> Widget)
	{
		PanelWidget::AppendSlot(Widget);

		// Create widget
		SharedPtr<Slot> PanelSlot = MakeShared(new Slot(Widget));
		ChildrenSlots.Add(PanelSlot);
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
			SharedPtr<Slot> ChildSlot = ChildrenSlots[ChildrenSlots.GetSize() - VBoxIndex - 1];

			SharedPtr<Ry::Widget> Widget = ChildSlot->GetWidget();
			SizeType ContentSize = Widget->ComputeSize();

			CurrentY += static_cast<int32>(ChildSlot->GetPadding().Bottom);

			// Set the widget's relative position
			Widget->SetRelativePosition(static_cast<float>(ChildSlot->GetPadding().Left), static_cast<float>(CurrentY));
			Widget->Arrange();

			CurrentY += static_cast<int32>(ContentSize.Height + ChildSlot->GetPadding().Top);

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

			for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
			{
				SizeType WidgetSize = ChildSlot->GetWidget()->ComputeSize();

				// Add horizontal padding
				WidgetSize.Width += (int32)(ChildSlot->GetPadding().Left + ChildSlot->GetPadding().Right);

				// Check if max width
				if (WidgetSize.Width > MaxChildWidth)
				{
					MaxChildWidth = WidgetSize.Width;
				}

				// Calculate height
				Result.Height += static_cast<int32>(ChildSlot->GetPadding().Top);
				Result.Height += static_cast<int32>(WidgetSize.Height);
				Result.Height += static_cast<int32>(ChildSlot->GetPadding().Bottom);
			}

			Result.Width += MaxChildWidth;
		}

		return Result;
	}

	void VerticalPanel::ClearChildren()
	{
		PanelWidget::ClearChildren();

		ChildrenSlots.Clear();
	}


}