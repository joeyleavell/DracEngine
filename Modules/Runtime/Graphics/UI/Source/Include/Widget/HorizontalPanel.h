#pragma once

#include "Widget/Panel.h"
#include "Core/Core.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE HorizontalLayout : public PanelWidget
	{
	public:

		WidgetBeginArgsSlot(HorizontalLayout)
			WidgetProp(float, SlotMargin)
		WidgetEndArgs()

		void Construct(Args& In)
		{

		}

		struct Slot : public PanelWidget::Slot
		{
			
		};

		static Slot MakeSlot()
		{
			Slot NewSlot;
			
			return NewSlot;
		}
		
		/**
		 * Arrange widgets horizontally.
		 */
		virtual void Arrange() override
		{
			// Default margin: 5px
			int32 CurrentX = static_cast<int32>(SlotMargin);
			int32 CurrentY = static_cast<int32>(SlotMargin);

			for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
			{
				SharedPtr<Ry::Widget> Widget = ChildSlot->Widget;
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
				for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
				{
					SizeType WidgetSize = ChildSlot->Widget->ComputeSize();

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

		void AppendSlot(Ry::SharedPtr<Ry::Widget>& Widget) override
		{
			PanelWidget::AppendSlot(Widget);

			// Create widget
			SharedPtr<Slot> PanelSlot = new Slot;
			PanelSlot->Widget = Widget;

			ChildrenSlots.Add(PanelSlot);			
		}

		void ClearChildren() override
		{
			PanelWidget::ClearChildren();

			ChildrenSlots.Clear();
		}

	private:

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

	};

}
