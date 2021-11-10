#pragma once

#include "Widget/Widget.h"
#include "Color.h"
#include "Drawable.h"
#include "Style.h"
#include "UserInterfaceGen.h"

namespace Ry
{
	class USERINTERFACE_MODULE SlotWidget : public Widget
	{
	public:

		WidgetBeginArgs(SlotWidget)
			WidgetProp(float, FillX)
			WidgetProp(float, FillY)
			WidgetPropDefault(float, Padding, 0.0f)
			WidgetPropDefault(VAlign, VerticalAlignment, VAlign::CENTER)
			WidgetPropDefault(HAlign, HorizontalAlignment, HAlign::CENTER)
		WidgetEndArgs()

		void Construct(Args& In)
		{
			this->PaddingLeft = PaddingRight = PaddingTop = PaddingBottom = In.mPadding;
			this->VerticalAlign = In.mVerticalAlignment;
			this->HorizontalAlign = In.mHorizontalAlignment;

			if (In.mFillX.IsSet())
			{
				this->FillX(In.mFillX);
			}

			if (In.mFillY.IsSet())
			{
				this->FillY(In.mFillY);
			}

			if(In.Children.GetSize() == 1)
			{
				SetChild(In.Children[0]);
			}
		}

		SlotWidget() :
			Widget()
		{
			this->PaddingLeft = PaddingRight = PaddingTop = PaddingBottom = 0;

			this->WidthMode = SizeMode::AUTO;
			this->HeightMode = SizeMode::AUTO;

			this->VerticalAlign = VAlign::TOP;
			this->HorizontalAlign = HAlign::LEFT;
		}

		virtual void SetParent(Widget* Parent) override
		{
			Widget::SetParent(Parent);

			// Update child's depth
			if(Child)
			{
				Child->SetParent(this);
			}
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

		void GetAllChildren(Ry::ArrayList<Widget*>& OutChildren) override
		{
			if(Child.IsValid())
			{
				OutChildren.Add(Child.Get());
				Child->GetAllChildren(OutChildren);
			}
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

			// Compute parent size only if we need to
			RectScissor ClipSpace = Widget::GetClipSpace(this);
			ParentSize.Width = ClipSpace.Width;
			ParentSize.Height = ClipSpace.Height;

			// if (Parent && (WidthMode == SizeMode::PERCENTAGE || HeightMode == SizeMode::PERCENTAGE))
			// {
			// 	ParentSize = Parent->ComputeSize();
			// }
			// else
			// {
			// }

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
				Child->SetVisible(bVisibility, true);
			}
		}

		void Draw(StyleSet* Style) override
		{			
			// Render child
			if (Child && Child->IsVisible())
			{
				Child->Draw(Style);
			}
		}

		void Arrange() override
		{
			if (Child)
			{
				SizeType ChildSize = Child->ComputeSize();
				float RelX = PaddingLeft;
				float RelY = PaddingBottom;

				if (WidthMode == SizeMode::PERCENTAGE)
				{
					RectScissor ThisSize = Widget::GetClipSpace(this);
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
					RectScissor ThisSize = Widget::GetClipSpace(this);
					switch ((uint32)VerticalAlign)
					{
					case (uint32)VAlign::CENTER:
						RelY = (ThisSize.Height - ChildSize.Height) / 2.0f;
						break;
					case (uint32)VAlign::TOP:
						RelY = ThisSize.Height - ChildSize.Height - PaddingTop;
						break;
					case (uint32)VAlign::BOTTOM:
						RelY = PaddingBottom;
						break;
					}
				}

				Child->SetRelativePosition(RelX, RelY);
				Child->Arrange();
			}
		}

/*		void ChildStateDirty(Widget* Wid, bool bFullRefresh)
		{
			Arrange();

			MarkDirty();
		}*/

		void SetChild(Ry::SharedPtr<Ry::Widget> Child)
		{
			// TODO: if there is an existing child, remove the parent/child links

			// If existing child, hide it
			if (this->Child)
			{
				this->Child->SetVisible(false, true);
			}
			
			// Setup the parent/child relationship
			this->Child = Child;
			Child->SetParent(this);
			Child->SetVisible(IsVisible(), true); // Child matches our visibility

			// Automatically rearrange
			Arrange();

			// Recompute cached size
			MarkDirty(this);
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

		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override
		{
			if (Child)
			{
				return Child->OnMouseScroll(MouseEv);
			}

			return false;
		}

		bool OnChar(const CharEvent& CharEv) override
		{
			if (Child)
			{
				return Child->OnChar(CharEv);
			}

			return false;
		}

		bool OnKey(const KeyEvent& KeyEv) override
		{
			if (Child)
			{
				return Child->OnKey(KeyEv);
			}

			return false;
		}

		Widget& operator[](SharedPtr<Ry::Widget> Child) override
		{
			SetChild(Child);

			return *this;
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
		SharedPtr<Ry::Widget> Child;

		/**
		 * The padding of the inner content of this box element.
		 */
		float PaddingLeft, PaddingRight, PaddingTop, PaddingBottom;

		VAlign VerticalAlign;
		HAlign HorizontalAlign;
	};
}
