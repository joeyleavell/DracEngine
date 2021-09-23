#pragma once

#include "Widget/Panel.h"
#include "UIGen.h"
#include "2D/Batch/Batch.h"
#include "Drawable.h"

namespace Ry
{

	// todo: cull widgets whose bounds are outside of scroll pane visibility
	class UI_MODULE ScrollPane : public PanelWidget
	{
	public:

		struct Slot : public PanelWidget::Slot
		{
			Slot() :
			PanelWidget::Slot()
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid):
			PanelWidget::Slot(Wid)
			{
			}

			ScrollPane::Slot& operator[](SharedPtr<Ry::Widget> Child)
			{
				this->Widget = Child;
				return *this;
			}

		};

		WidgetBeginArgsSlot(ScrollPane)
			WidgetProp(int32, Width)
			WidgetProp(int32, Height)
		WidgetEndArgs()

		void Construct(Args& In)
		{

			for (PanelWidget::Slot& Child : In.Slots)
			{
				AppendSlot(Child.GetWidget());
			}

			this->Size.Width  = In.mWidth;
			this->Size.Height = In.mHeight;

			this->bAllowHorizontalScroll = true;

			this->VerticalScrollAmount   = 0.0f;
			this->HorizontalScrollAmount = 0.0f;
			this->ScrollBarThickness     = 20.0f;
			this->bVerticalBarPressed    = false;
			this->bHorizontalBarPressed  = false;

			VerticalScrollBar = MakeShared(new BoxDrawable);
			VerticalScrollBar->SetBackgroundColor(WHITE.ScaleRGB(0.4f));
			VerticalScrollBar->SetBorderRadius(0);
			VerticalScrollBar->SetBorderSize(0);

			HorizontalScrollBar = MakeShared(new BoxDrawable);
			HorizontalScrollBar->SetBackgroundColor(WHITE.ScaleRGB(0.4f));
			HorizontalScrollBar->SetBorderRadius(0);
			HorizontalScrollBar->SetBorderSize(0);

			DebugRect = Ry::MakeItem();
		}

		void SetBatch(Batch* Bat) override
		{
			PanelWidget::SetBatch(Bat);
			
			if(Bat)
			{
				VerticalScrollBar->Bat = Bat;
				HorizontalScrollBar->Bat = Bat;
			}
		}

		float GetVerticalScrollAmount()
		{
			return VerticalScrollAmount;
		}

		float GetHorizontalScrollAmount()
		{
			return HorizontalScrollAmount;
		}

		void VerticalScroll(float Pixels)
		{
			SizeType ChildrenSize = ComputeChildrenSize();
			float HiddenAmountY = ChildrenSize.Height - Size.Height;

			SetVerticalScrollAmount(VerticalScrollAmount + Pixels / HiddenAmountY);
		}

		void HorizontalScroll(float Pixels)
		{
			SizeType ChildrenSize = ComputeChildrenSize();
			float HiddenAmountX = ChildrenSize.Width - Size.Width;

			SetHorizontalScrollAmount(HorizontalScrollAmount + Pixels / HiddenAmountX);
		}

		void SetHorizontalScrollAmount(float Scroll)
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
				MarkDirty();
			}
		}

		void SetVerticalScrollAmount(float Scroll)
		{
			float Prev = this->VerticalScrollAmount;
			this->VerticalScrollAmount = Scroll;

			if (VerticalScrollAmount > 1.0f)
				VerticalScrollAmount = 1.0f;
			else if (VerticalScrollAmount < 0.0f)
				VerticalScrollAmount = 0.0f;

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

		virtual SharedPtr<PanelWidget::Slot> AppendSlot(SharedPtr<Widget> Widget) override
		{
			PanelWidget::AppendSlot(Widget);

			// Create slot
			SharedPtr<Slot> ScrollSlot = MakeShared(new Slot(Widget));
			ChildrenSlots.Add(ScrollSlot);

			return ScrollSlot;
		}

		void Draw() override
		{
			Point Abs = GetAbsolutePosition();
			Ry::BatchHollowRectangle(DebugRect, WHITE, Abs.X + 2.0f, Abs.Y + 2.0f, Size.Width - 4.0f, Size.Height - 4.0f, 2.0f, 0.0f);

			Point HorLoc;
			Point VertLoc;
			SizeType HorSize;
			SizeType VertSize;

			GetVertScrollBarBounds(VertLoc, VertSize);
			GetHorScrollBarBounds(HorLoc, HorSize);

			VerticalScrollBar->Draw(VertLoc.X, VertLoc.Y, VertSize.Width, VertSize.Height);
			HorizontalScrollBar->Draw(HorLoc.X, HorLoc.Y, HorSize.Width, HorSize.Height);

			Ry::PanelWidget::Draw();
		}

		/**
		 * Arrange widgets vertically.
		 */
		virtual void Arrange() override
		{
			RectScissor Clip = GetClipSpace();
			if(!(Clip == LastClip))
			{
				UpdatePipelineState();
			}
			LastClip = Clip;
			
			// Calculate scroll amount
			SizeType ChildrenSize = ComputeChildrenSize();
			float HiddenAmountX = ChildrenSize.Width - Size.Width;
			float HiddenAmountY = ChildrenSize.Height - Size.Height;
			int32 OffsetX = HiddenAmountX * HorizontalScrollAmount;
			int32 OffsetY = HiddenAmountY * VerticalScrollAmount;
			if (OffsetX < 0)
				OffsetX = 0;
			if (OffsetY < 0)
				OffsetY = 0;
			
			int32 CurrentY = static_cast<int32>(0);

			for (SharedPtr<Slot> ChildSlot : ChildrenSlots)
			{
				SharedPtr<Ry::Widget> Widget = ChildSlot->GetWidget();
				SizeType ContentSize = Widget->ComputeSize();

				// Add top padding prior to setting relative position
				CurrentY += static_cast<int32>(ChildSlot->GetPadding().Top);

				float WidgetX = static_cast<float>(ChildSlot->GetPadding().Left - OffsetX);
				float WidgetY = static_cast<float>(CurrentY - OffsetY);

				// Set the widget's relative position
				Widget->SetRelativePosition(WidgetX, WidgetY);
				Widget->Arrange();

				CurrentY += static_cast<int32>(ContentSize.Height + ChildSlot->GetPadding().Bottom);
			}

		}

		PipelineState GetPipelineState() const override
		{
			PipelineState State;
			State.Scissor = GetClipSpace();
			State.StateID = GetWidgetID();

			return State;
		}

		virtual RectScissor GetClipSpace() const override
		{			
			Point Position = GetAbsolutePosition();

			RectScissor Res;
			Res.X      = Position.X;
			Res.Y      = Position.Y;
			Res.Width  = Size.Width;
			Res.Height = Size.Height;

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

			return Res;
		}

		virtual void OnShow() override
		{
			Widget::OnShow();

			if(Bat)
			{
				Bat->AddItem(DebugRect, "Shape", GetPipelineState());
			}

			// Push scroll bar to last layer
			// todo: come up with better way than this
			RectScissor Scissor;
			HorizontalScrollBar->Show(-1, GetPipelineState());
			VerticalScrollBar->Show(-1, GetPipelineState());
		}

		virtual void OnHide() override
		{
			Widget::OnHide();

			if(Bat)
			{
				Bat->RemoveItem(DebugRect);
			}

			VerticalScrollBar->Hide();
			HorizontalScrollBar->Hide();
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
			bool bChildHandle = PanelWidget::OnMouseScroll(MouseEv);

			if(!bChildHandle && !bVerticalBarPressed && IsHovered())
			{
				float ScrollY = MouseEv.ScrollY;

				std::cout << ScrollY << std::endl;

				VerticalScroll(-ScrollY * 30.0f);

				return true;
			}

			return bChildHandle;
		}

		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override
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

			if(MouseEv.ButtonID == 0)
			{
				// Mouse must be inside scroll bar rect to initiate drag
				if(MouseEv.bPressed)
				{
					if(bVertBoundsTest && !bVerticalBarPressed)
					{
						// Start dragging vertical scroll bar
						bVerticalBarPressed = true;

						// Store the initial delta Y position
						RelativeScrollBarPressed.Y = MouseEv.MouseY - VertLoc.Y;
					}
					else if(bHorBoundsTest && !bHorizontalBarPressed)
					{
						// Start dragging vertical scroll bar
						bHorizontalBarPressed = true;

						// Store the initial delta Y position
						RelativeScrollBarPressed.X = MouseEv.MouseX - HorLoc.X;
					}
				}

				// Mouse must not continue to stay in scroll bar
				if(!MouseEv.bPressed)
				{
					bVerticalBarPressed = false;
					bHorizontalBarPressed = false;
				}
				
				return true;
			}

			return PanelWidget::OnMouseButtonEvent(MouseEv);
		}

		bool OnMouseEvent(const MouseEvent& MouseEv) override
		{
			if(bVerticalBarPressed)
			{
				Point VertAbs;
				SizeType VertSize;
				GetVertScrollBarBounds(VertAbs, VertSize);
				float ScrollRange = Size.Height - VertSize.Height;

				// get new location
				float CurY = MouseEv.MouseY;
				float Adjusted = CurY - RelativeScrollBarPressed.Y;
				float Relative = Adjusted - GetAbsolutePosition().Y;

				if (Relative < 0)
					Relative = 0;
				if (Relative > ScrollRange)
					Relative = ScrollRange;

				if(ScrollRange > 0.01f)
				{
					SetVerticalScrollAmount(Relative / ScrollRange);
				}
				
				return true;
			}

			if(bHorizontalBarPressed)
			{
				Point HorAbs;
				SizeType HorSize;
				GetHorScrollBarBounds(HorAbs, HorSize);
				float ScrollRange = (Size.Width - ScrollBarThickness) - HorSize.Width;

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

	private:

		void GetVertScrollBarBounds(Point& OutPos, SizeType& OutSize)
		{
			Point Abs = GetAbsolutePosition();
			SizeType ChildrenSize = ComputeChildrenSize();
			OutSize.Width = ScrollBarThickness;
			OutSize.Height = Size.Height * std::min(Size.Height / (float)ChildrenSize.Height, 1.0f);

			float ScrollRange = Size.Height - OutSize.Height;
			OutPos.X = Abs.X + Size.Width - ScrollBarThickness;
			OutPos.Y = Abs.Y + ScrollRange * VerticalScrollAmount;
		}

		void GetHorScrollBarBounds(Point& OutPos, SizeType& OutSize)
		{
			Point Abs = GetAbsolutePosition();
			SizeType ChildrenSize = ComputeChildrenSize();
			OutSize.Width = (Size.Width - ScrollBarThickness) * std::min((Size.Width - ScrollBarThickness) / (float)ChildrenSize.Width, 1.0f);
			OutSize.Height = ScrollBarThickness;

			float ScrollRange = (Size.Width - ScrollBarThickness) - OutSize.Width;
			OutPos.X = Abs.X + ScrollRange * HorizontalScrollAmount;
			OutPos.Y = Abs.Y + Size.Height - ScrollBarThickness;
		}

		// Utility to compute the size of the children embedded within the scroll pane
		SizeType ComputeChildrenSize() const
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
					WidgetSize.Width += ChildSlot->GetPadding().Left + ChildSlot->GetPadding().Right;

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


		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;
		SizeType Size;

		Ry::SharedPtr<BatchItem> DebugRect;

		float VerticalScrollAmount;
		float HorizontalScrollAmount;

		float ScrollBarThickness;

		Ry::SharedPtr<BoxDrawable> VerticalScrollBar;
		Ry::SharedPtr<BoxDrawable> HorizontalScrollBar;

		bool bVerticalBarPressed;
		bool bHorizontalBarPressed;
		Point RelativeScrollBarPressed;

		bool bAllowHorizontalScroll;

		RectScissor LastClip;
	};

}
