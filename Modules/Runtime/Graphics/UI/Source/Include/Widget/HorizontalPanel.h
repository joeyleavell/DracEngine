#pragma once

#include "Widget/Panel.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE HorizontalLayout : public PanelWidget
	{
	public:

		struct Slot
		{
			Ry::Widget* Widget;
			float LeftMargin;
			float RightMargin;
			float TopMargin;
			float BottomMargin;
		};
		
		/**
		 * Arrange widgets horizontally.
		 */
		virtual void Arrange() override
		{
			// Default margin: 5px
			int32 CurrentX = static_cast<int32>(SlotMargin);
			int32 CurrentY = static_cast<int32>(SlotMargin);

			for (const Slot& Slot : ChildrenSlots)
			{
				Ry::Widget* Widget = Slot.Widget;
				SizeType ContentSize = Widget->ComputeSize();

				// Set the widget's relative position
				Widget->SetRelativePosition(static_cast<float>(CurrentX), static_cast<float>(CurrentY));
				Widget->Arrange();

				CurrentX += static_cast<int32>(ContentSize.Width + SlotMargin);
			}
		}

		SizeType ComputeSize() const override
		{
			SizeType Result;
			Result.Width = 0;
			Result.Height = 0;

			// Height is 2 * Margin + MaxChildHeight

			if (!ChildrenSlots.IsEmpty())
			{
				// Initial margins
				Result.Width = static_cast<int32>(SlotMargin);
				Result.Height = static_cast<int32>(2 * SlotMargin);

				int32 MaxChildHeight = 0;

				for (const Slot& Slot : ChildrenSlots)
				{
					SizeType WidgetSize = Slot.Widget->ComputeSize();

					if (WidgetSize.Height > MaxChildHeight)
					{
						MaxChildHeight = WidgetSize.Height;
					}

					Result.Width += static_cast<int32>(WidgetSize.Width + SlotMargin);
				}

				Result.Height += MaxChildHeight;
			}

			return Result;
		}

		void AppendSlot(Ry::Widget& Widget) override
		{
			PanelWidget::AppendSlot(Widget);

			// Create widget
			Slot PanelSlot;
			PanelSlot.Widget = &Widget;

			ChildrenSlots.Add(PanelSlot);
		}

		void ClearChildren() override
		{
			PanelWidget::ClearChildren();

			ChildrenSlots.Clear();
		}

	private:

		Ry::ArrayList<Slot> ChildrenSlots;

	};

}
