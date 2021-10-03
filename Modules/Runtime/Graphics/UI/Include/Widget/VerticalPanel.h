#pragma once

#include "Widget/Panel.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE VerticalLayout : public PanelWidget
	{		
	public:

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

			VerticalLayout::Slot& operator[](SharedPtr<Ry::Widget> Child)
			{
				this->Widget = Child;
				return *this;
			}

		};

		WidgetBeginArgsSlot(VerticalLayout)
			WidgetProp(float, Pad)
		WidgetEndArgs()

		void Construct(Args& In)
		{
			for (PanelWidget::Slot& Child : In.Slots)
			{
				AppendSlot(Child.GetWidget());
			}
		}

		static VerticalLayout::Slot MakeSlot()
		{
			VerticalLayout::Slot NewSlot;
			return NewSlot;
		}

		virtual Ry::SharedPtr<PanelWidget::Slot> AppendSlot(Ry::SharedPtr<Ry::Widget> Widget) override
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
		virtual void Arrange() override
		{
			// Default margin: 5px
			int32 CurrentX = static_cast<int32>(0);
			int32 CurrentY = static_cast<int32>(0);

			for(int32 VBoxIndex = 0; VBoxIndex < ChildrenSlots.GetSize(); VBoxIndex++)
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

		SizeType ComputeSize() const override
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
					WidgetSize.Width += (int32) (ChildSlot->GetPadding().Left + ChildSlot->GetPadding().Right);

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

		void ClearChildren() override
		{
			PanelWidget::ClearChildren();

			ChildrenSlots.Clear();
		}
		
	private:

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

	};

}
