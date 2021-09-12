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
		WidgetEndArgs()

		struct Slot : public PanelWidget::Slot
		{
			Slot() :
			PanelWidget::Slot()
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid) :
			PanelWidget::Slot(Wid)
			{
			}
		};

		void Construct(HorizontalLayout::Args& In)
		{
			for(PanelWidget::Slot& Child : In.Slots)
			{
				SharedPtr<Slot> Result = CastShared<Slot>(AppendSlot(Child.GetWidget()));
				Result->SetPadding(Child.GetPadding());
			}
		}

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
			int32 CurrentX = static_cast<int32>(0);

			for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
			{
				SharedPtr<Ry::Widget> Widget = ChildSlot->GetWidget();
				SizeType ContentSize = Widget->ComputeSize();

				CurrentX += static_cast<int32>(ChildSlot->GetPadding().Left);

				// Set the widget's relative position
				Widget->SetRelativePosition(static_cast<float>(CurrentX), static_cast<float>(ChildSlot->GetPadding().Top));
				Widget->Arrange();

				CurrentX += static_cast<int32>(ContentSize.Width + ChildSlot->GetPadding().Right);
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
				int32 MaxChildHeight = 0;
				for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
				{
					SizeType WidgetSize = ChildSlot->GetWidget()->ComputeSize();

					// Add vertical padding
					WidgetSize.Height += ChildSlot->GetPadding().Top + ChildSlot->GetPadding().Bottom;
					
					if (WidgetSize.Height > MaxChildHeight)
					{
						MaxChildHeight = WidgetSize.Height;
					}

					// Calculate width
					Result.Width += static_cast<int32>(ChildSlot->GetPadding().Left);
					Result.Width += static_cast<int32>(WidgetSize.Width);
					Result.Width += static_cast<int32>(ChildSlot->GetPadding().Right);
				}

				Result.Height = MaxChildHeight;
			}

			return Result;
		}

		virtual Ry::SharedPtr<PanelWidget::Slot> AppendSlot(Ry::SharedPtr<Ry::Widget> Widget) override
		{
			PanelWidget::AppendSlot(Widget);

			// Create widget
			SharedPtr<Slot> PanelSlot = new Slot(Widget);
			ChildrenSlots.Add(PanelSlot);
			return PanelSlot;
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
