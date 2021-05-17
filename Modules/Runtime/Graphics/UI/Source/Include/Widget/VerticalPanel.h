#pragma once

#include "Widget/Panel.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE VerticalLayout : public PanelWidget
	{		
	public:

		struct Slot
		{
			SharedPtr<Ry::Widget> Widget;
			// float LeftMargin;
			// float RightMargin;
			// float TopMargin;
			// float BottomMargin;
		};

		void AppendSlot(Ry::Widget& Widget) override
		{
			PanelWidget::AppendSlot(Widget);

			// Create widget
			Slot PanelSlot;
			PanelSlot.Widget = &Widget;

			ChildrenSlots.Add(PanelSlot);
		}
		
		/**
		 * Arrange widgets vertically.
		 */
		virtual void Arrange() override
		{
			// Default margin: 5px
			int32 CurrentX = static_cast<int32>(SlotMargin);
			int32 CurrentY = static_cast<int32>(SlotMargin);

			for (const Slot& Slot : ChildrenSlots)
			{
				SharedPtr<Ry::Widget> Widget = Slot.Widget;
				SizeType ContentSize = Widget->ComputeSize();

				// Set the widget's relative position
				Widget->SetRelativePosition(static_cast<float>(CurrentX), static_cast<float>(CurrentY));
				Widget->Arrange();
				
				CurrentY += static_cast<int32>(ContentSize.Height + SlotMargin);
			}
		}

		SizeType ComputeSize() const override
		{
			SizeType Result;
			Result.Width = 0;
			Result.Height = 0;

			// Width is 2 * Margin + MaxChildWidth

			if (!ChildrenSlots.IsEmpty())
			{
				// Initial margins
				Result.Width = static_cast<int32>(2 * SlotMargin);
				Result.Height = static_cast<int32>(SlotMargin);

				int32 MaxChildWidth = 0;

				for (const Slot& Slot : ChildrenSlots)
				{
					SizeType WidgetSize = Slot.Widget->ComputeSize();

					if (WidgetSize.Width > MaxChildWidth)
					{
						MaxChildWidth = WidgetSize.Width;
					}

					Result.Height += static_cast<int32>(WidgetSize.Height + SlotMargin);
				}

				Result.Width += MaxChildWidth;
			}

			return Result;
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
