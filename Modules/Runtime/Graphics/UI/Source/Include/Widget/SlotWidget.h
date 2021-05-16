#pragma once

#include "Widget/Widget.h"
#include "Color.h"
#include "Drawable.h"
#include "Style.h"
#include "UIGen.h"

namespace Ry
{
	class UI_MODULE SlotWidget : public Widget
	{
	public:

		SlotWidget() :
			Widget()
		{
			// No child initially
			this->Child = nullptr;

			this->PaddingLeft = PaddingRight = PaddingTop = PaddingBottom = 0;

			this->WidthMode = SizeMode::AUTO;
			this->HeightMode = SizeMode::AUTO;

			this->VerticalAlign = VAlign::TOP;
			this->HorizontalAlign = HAlign::LEFT;
		}

		SlotWidget& Padding(float Pad)
		{
			return Padding(Pad, Pad);
		}

		SlotWidget& Padding(float Vertical, float Horizontal)
		{
			return Padding(Horizontal, Horizontal, Vertical, Vertical);
		}

		SlotWidget& Padding(float Left, float Right, float Top, float Bottom)
		{
			this->PaddingLeft = Left;
			this->PaddingRight = Right;
			this->PaddingTop = Top;
			this->PaddingBottom = Bottom;

			return *this;
		}

		SlotWidget& SetVerticalAlignment(VAlign Alignment)
		{
			this->ContentVAlign = Alignment;

			return *this;
		}

		SlotWidget& SetHorizontalAlignment(HAlign Alignment)
		{
			this->ContentHAlign = Alignment;

			return *this;
		}

		SlotWidget& AutoWidth()
		{
			this->WidthMode = SizeMode::AUTO;

			return *this;
		}

		SlotWidget& AutoHeight()
		{
			this->WidthMode = SizeMode::AUTO;

			return *this;
		}

		SlotWidget& FillX(float FillX)
		{
			this->FillXPercent = FillX;
			this->WidthMode = SizeMode::PERCENTAGE;

			return *this;
		}

		SlotWidget& FillY(float FillY)
		{
			this->FillYPercent = FillY;
			this->HeightMode = SizeMode::PERCENTAGE;

			return *this;
		}

		SlotWidget& FillParent()
		{
			return FillX(1.0).FillY(1.0);
		}

		SlotWidget& SetVAlign(VAlign VerticalAlign)
		{
			this->VerticalAlign = VerticalAlign;

			return *this;
		}

		SlotWidget& SetHAlign(HAlign HorizontalAlign)
		{
			this->HorizontalAlign = HorizontalAlign;

			return *this;
		}

		SizeType ComputeSize() const override
		{
			SizeType ParentSize;
			SizeType ChildSize;
			SizeType Result;

			// Compute child size, only if needed otherwise we'd enter infinite recursion
			if (Child && (WidthMode == SizeMode::AUTO || HeightMode == SizeMode::AUTO))
			{
				ChildSize = Child->ComputeSize();
			}

			// Compute parent size
			// todo: why the added &&? 
			if (Parent && (WidthMode == SizeMode::PERCENTAGE || HeightMode == SizeMode::PERCENTAGE))
			{
				ParentSize = Parent->ComputeSize();
			}
			else
			{
				ParentSize.Width = Ry::GetViewportWidth();
				ParentSize.Height = Ry::GetViewportHeight();
			}

			if (WidthMode == SizeMode::PERCENTAGE)
			{
				Result.Width = (int32)(ParentSize.Width * FillXPercent);
			}
			else if (WidthMode == SizeMode::AUTO)
			{
				Result.Width = (int32)(PaddingLeft + PaddingRight + ChildSize.Width);
			}

			if (HeightMode == SizeMode::PERCENTAGE)
			{
				Result.Height = (int32)(ParentSize.Height * FillYPercent);
			}
			else if (HeightMode == SizeMode::AUTO)
			{
				Result.Height = (int32)(PaddingTop + PaddingBottom + ChildSize.Height);
			}

			return Result;
		}

		void SetVisible(bool bVisibility, bool bPropagate) override
		{
			Widget::SetVisible(bVisibility, bPropagate);

			if (bPropagate && Child)
			{
				Child->SetVisible(bVisibility, bPropagate);
			}
		}

		void Draw() override
		{			
			// Render child
			if (Child)
			{
				Child->Draw();
			}
		}

		void Arrange() override
		{
			if (Child)
			{
				SizeType ChildSize = Child->ComputeSize();
				float RelX = PaddingLeft;
				float RelY = PaddingTop;

				if (WidthMode == SizeMode::PERCENTAGE)
				{
					SizeType ThisSize = ComputeSize();
					switch ((uint32)HorizontalAlign)
					{
					case (uint32)HAlign::CENTER:
						RelX = (ThisSize.Width - ChildSize.Width) / 2.0f;
						break;
					case (uint32)HAlign::LEFT:
						RelX = PaddingLeft;
						break;
					case (uint32)HAlign::RIGHT:
						RelX = (float)(ThisSize.Width - ChildSize.Width - PaddingRight);
						break;
					}
				}

				if (HeightMode == SizeMode::PERCENTAGE)
				{
					SizeType ThisSize = ComputeSize();
					switch ((uint32)VerticalAlign)
					{
					case (uint32)VAlign::CENTER:
						RelY = (ThisSize.Height - ChildSize.Height) / 2.0f;
						break;
					case (uint32)VAlign::TOP:
						RelY = PaddingTop;
						break;
					case (uint32)VAlign::BOTTOM:
						RelY = ThisSize.Height - ChildSize.Height - PaddingBottom;
						break;
					}
				}

				Child->SetRelativePosition(RelX, RelY);
				Child->Arrange();
			}
		}

		void ChildStateDirty()
		{
			Arrange();

			MarkDirty();
		}

		void SetChild(Ry::Widget& Child)
		{
			// TODO: if there is an existing child, remove the parent/child links

			// If existing child, hide it
			if (this->Child)
			{
				this->Child->SetVisible(false, true);
			}

			// Set existing batches
			// TODO: this should just be a pointer back up to user interface
			Child.SetShapeBatch(ShapeBatch);
			Child.SetTextureBatch(TextureBatch);
			Child.SetTextBatch(TextBatch);
			
			// Setup the parent/child relationship
			this->Child = &Child;
			Child.SetParent(this);
			Child.SetVisible(IsVisible(), true); // Child matches our visibility
			Child.RenderStateDirty.AddMemberFunction(this, &SlotWidget::ChildStateDirty);

			// Automatically rearrange
			Arrange();

			// Recompute cached size
			MarkDirty();
		}

		bool OnMouseEvent(const MouseEvent& MouseEv) override
		{
			bool bHandled = Widget::OnMouseEvent(MouseEv);

			if (Child)
			{
				Child->OnMouseEvent(MouseEv);
			}

			return bHandled;
		}

		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override
		{
			bool bHandled = Widget::OnMouseButtonEvent(MouseEv);

			if (Child)
			{
				bHandled |= Child->OnMouseButtonEvent(MouseEv);
			}

			return bHandled;
		}

		bool OnMouseClicked(const MouseClickEvent& MouseEv) override
		{
			if(Child)
			{
				return Child->OnMouseClicked(MouseEv);
			}

			return false;
		}

		bool OnMouseDragged(const MouseDragEvent& MouseEv) override
		{
			if (Child)
			{
				return Child->OnMouseDragged(MouseEv);
			}

			return false;
		}

		Widget& operator[](Ry::Widget& Child) override
		{
			SetChild(Child);

			return *this;
		}

		void SetShapeBatch(Batch* Shape) override
		{
			Widget::SetShapeBatch(Shape);

			if (Child)
			{
				Child->SetShapeBatch(Shape);
			}

		}

		void SetTextBatch(Batch* Text) override
		{
			Widget::SetTextBatch(Text);

			if (Child)
			{
				Child->SetTextBatch(Text);
			}

		}

		void SetTextureBatch(Batch* Text) override
		{
			Widget::SetTextureBatch(Text);

			if (Child)
			{
				Child->SetTextureBatch(Text);
			}
		}

		void OnHovered(const MouseEvent& MouseEv) override
		{
			Widget::OnHovered(MouseEv);
		}

		void OnUnhovered(const MouseEvent& MouseEv) override
		{
			Widget::OnUnhovered(MouseEv);
		}

		bool OnPressed(const MouseButtonEvent& MouseEv) override
		{
			Widget::OnPressed(MouseEv);

			return true;
		}

		bool OnReleased(const MouseButtonEvent& MouseEv) override
		{
			Widget::OnReleased(MouseEv);

			return true;
		}

	private:

		VAlign ContentVAlign;
		HAlign ContentHAlign;
		SizeMode WidthMode;
		SizeMode HeightMode;

		// Percentage parameters
		float FillXPercent;
		float FillYPercent;

		/**
		 * The child stored within this box element.
		 */
		Ry::Widget* Child;

		/**
		 * The padding of the inner content of this box element.
		 */
		float PaddingLeft, PaddingRight, PaddingTop, PaddingBottom;

		VAlign VerticalAlign;
		HAlign HorizontalAlign;
	};
}
