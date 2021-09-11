#pragma once

#include "Widget/Panel.h"
#include "UIGen.h"
#include "2D/Batch/Batch.h"

namespace Ry
{

	class UI_MODULE ScrollPane : public PanelWidget
	{
	public:

		struct Slot : public PanelWidget::Slot
		{
			Slot& operator[](SharedPtr<Ry::Widget> Child)
			{
				this->Widget = Child;
				return *this;
			}
		};

		WidgetBeginArgsSlot(ScrollPane)
			WidgetProp(float, SlotMargin)
			WidgetProp(int32, Width)
			WidgetProp(int32, Height)
		WidgetEndArgs()

		void Construct(Args& In)
		{
			this->SlotMargin = In.mSlotMargin;

			for (PanelWidget::Slot& Child : In.Slots)
			{
				AppendSlot(Child.Widget);
			}

			this->Size.Width  = In.mWidth;
			this->Size.Height = In.mHeight;

			this->ScrollAmount = 0.0f;
		}

		float GetScrollAmount()
		{
			return ScrollAmount;
		}

		void SetScrollAmount(float Scroll)
		{
			float Prev = this->ScrollAmount;
			this->ScrollAmount = Scroll;

			std::cout << this->ScrollAmount << std::endl;

			if (ScrollAmount > 1.0f)
				ScrollAmount = 1.0f;
			else if (ScrollAmount < 0.0f)
				ScrollAmount = 0.0f;

			if(std::abs(Prev - Scroll) >= 0.000001f)
			{
				// Scroll amount has changed, mark widget as dirty	
				MarkDirty();
			}
		}

		static ScrollPane::Slot MakeSlot()
		{
			Slot NewSlot;
			return NewSlot;
		}

		void AppendSlot(Ry::SharedPtr<Ry::Widget>& Widget) override
		{
			PanelWidget::AppendSlot(Widget);

			// Create widget
			SharedPtr<Slot> PanelSlot = new Slot;
			PanelSlot->Widget = Widget;

			ChildrenSlots.Add(PanelSlot);
		}

		/**
		 * Arrange widgets vertically.
		 */
		virtual void Arrange() override
		{
			SizeType ChildrenSize = ComputeChildrenSize();
			float HiddenAmount = ChildrenSize.Height - Size.Height;
			int32 OffsetY = HiddenAmount * ScrollAmount;
			if (OffsetY < 0)
				OffsetY = 0;
			
			// Default margin: 5px
			int32 CurrentX = static_cast<int32>(SlotMargin);
			int32 CurrentY = static_cast<int32>(SlotMargin);

			for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
			{
				SharedPtr<Ry::Widget> Widget = ChildSlot->Widget;
				SizeType ContentSize = Widget->ComputeSize();

				// Set the widget's relative position
				Widget->SetRelativePosition(static_cast<float>(CurrentX), static_cast<float>(CurrentY - OffsetY));
				Widget->Arrange();

				CurrentY += static_cast<int32>(ContentSize.Height + SlotMargin);
			}

		}

		virtual RectScissor GetClipSpace() override
		{			
			Point Position = GetAbsolutePosition();

			RectScissor Res;
			Res.X      = Position.X;
			Res.Y      = Position.Y;
			Res.Width  = Size.Width;
			Res.Height = Size.Height;

			if (Res.Y < 0)
				Res.Y = 0;
			if (Res.X < 0)
				Res.X = 0;

			return Res;
		}

		virtual void OnShow() override
		{
			Widget::OnShow();

			if(!DebugRect)
			{
				DebugRect = Ry::MakeItem();
			}

			Point Abs = GetAbsolutePosition();
			Ry::BatchHollowRectangle(DebugRect, WHITE, Abs.X, Abs.Y, Size.Width, Size.Height, 5.0f, 0.0f);
			if(Bat)
			{
				RectScissor Scissor;
				Bat->AddItem(DebugRect, "Shape", Scissor, nullptr, WidgetLayer);
			}
		}

		virtual void OnHide() override
		{
			Widget::OnHide();

			Bat->RemoveItem(DebugRect);
		}

		SizeType ComputeSize() const override
		{
			return Size;
		}

		void ClearChildren() override
		{
			PanelWidget::ClearChildren();

			ChildrenSlots.Clear();
		}

		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override
		{
			float ScrollY = MouseEv.ScrollY;

			SetScrollAmount(ScrollAmount - ScrollY / 30.0f);

			return true;
		}

	private:

		// Utility to compute the size of the children embedded within the scroll pane
		SizeType ComputeChildrenSize() const
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

				for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
				{
					SizeType WidgetSize = ChildSlot->Widget->ComputeSize();

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


		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;
		SizeType Size;

		Ry::SharedPtr<BatchItem> DebugRect;

		float ScrollAmount;

	};

}
