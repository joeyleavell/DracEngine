#include "Widget/Layout/SlotWidget.h"

namespace Ry
{

	void SlotWidget::Construct(Args& In)
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

		if (In.Children.GetSize() == 1)
		{
			SetChild(In.Children[0]);
		}
	}

	SharedPtr<Widget> SlotWidget::FindChildWidgetById(const Ry::String& Id) const
	{
		if(Child)
		{
			if (Id == Child->GetId())
				return Child;
			else
				return Child->FindChildWidgetById(Id);
		}

		return SharedPtr<Ry::Widget>();
	}

	void SlotWidget::SetStyle(const Ry::StyleSet* Style)
	{
		Widget::SetStyle(Style);

		if(Child.IsValid())
		{
			Child->SetStyle(Style);
		}
	}

	SlotWidget::SlotWidget() :
		Widget()
	{
		this->PaddingLeft = PaddingRight = PaddingTop = PaddingBottom = 0;

		this->WidthMode = SizeMode::AUTO;
		this->HeightMode = SizeMode::AUTO;

		this->VerticalAlign = VAlign::TOP;
		this->HorizontalAlign = HAlign::LEFT;
	}

	void SlotWidget::SetParent(Widget* Parent)
	{
		Widget::SetParent(Parent);

		// Update child's depth
		if (Child)
		{
			Child->SetParent(this);
		}
	}

	SlotWidget& SlotWidget::Padding(float Pad)
	{
		return Padding(Pad, Pad);
	}

	SlotWidget& SlotWidget::Padding(float Vertical, float Horizontal)
	{
		return Padding(Horizontal, Horizontal, Vertical, Vertical);
	}

	SlotWidget& SlotWidget::Padding(float Left, float Right, float Top, float Bottom)
	{
		this->PaddingLeft = Left;
		this->PaddingRight = Right;
		this->PaddingTop = Top;
		this->PaddingBottom = Bottom;

		return *this;
	}

	SlotWidget& SlotWidget::SetVerticalAlignment(VAlign Alignment)
	{
		this->ContentVAlign = Alignment;

		return *this;
	}

	SlotWidget& SlotWidget::SetHorizontalAlignment(HAlign Alignment)
	{
		this->ContentHAlign = Alignment;

		return *this;
	}

	SlotWidget& SlotWidget::AutoWidth()
	{
		this->WidthMode = SizeMode::AUTO;

		return *this;
	}

	SlotWidget& SlotWidget::AutoHeight()
	{
		this->WidthMode = SizeMode::AUTO;

		return *this;
	}

	SlotWidget& SlotWidget::FillX(float FillX)
	{
		this->FillXPercent = FillX;
		this->WidthMode = SizeMode::PERCENTAGE;

		return *this;
	}

	SlotWidget& SlotWidget::FillY(float FillY)
	{
		this->FillYPercent = FillY;
		this->HeightMode = SizeMode::PERCENTAGE;

		return *this;
	}

	SlotWidget& SlotWidget::FillParent()
	{
		return FillX(1.0).FillY(1.0);
	}

	SlotWidget& SlotWidget::SetVAlign(VAlign VerticalAlign)
	{
		this->VerticalAlign = VerticalAlign;

		return *this;
	}

	SlotWidget& SlotWidget::SetHAlign(HAlign HorizontalAlign)
	{
		this->HorizontalAlign = HorizontalAlign;

		return *this;
	}

	void SlotWidget::GetAllChildren(Ry::ArrayList<Widget*>& OutChildren)
	{
		if (Child.IsValid())
		{
			OutChildren.Add(Child.Get());
			Child->GetAllChildren(OutChildren);
		}
	}

	SizeType SlotWidget::ComputeSize() const
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

	void SlotWidget::SetVisible(bool bVisibility, bool bPropagate)
	{
		Widget::SetVisible(bVisibility, bPropagate);

		if (bPropagate && Child)
		{
			Child->SetVisible(bVisibility, true);
		}
	}

	void SlotWidget::Draw()
	{
		// Render child
		if (Child && Child->IsVisible())
		{
			Child->Draw();
		}
	}

	void SlotWidget::Arrange()
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

	void SlotWidget::SetChild(Ry::SharedPtr<Ry::Widget> Child)
	{
		// TODO: if there is an existing child, remove the parent/child links

		// If existing child, hide it
		if (this->Child)
		{
			this->Child->SetVisible(false, true);
		}

		// Setup the parent/child relationship
		this->Child = Child;
		Child->SetStyle(Style);
		Child->SetParent(this);
		Child->SetVisible(IsVisible(), true); // Child matches our visibility

		// Automatically rearrange
		Arrange();

		// Recompute cached size
		MarkDirty(this);
	}

	bool SlotWidget::OnMouseEvent(const MouseEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseEvent(MouseEv);

		if (Child)
		{
			Child->OnMouseEvent(MouseEv);
		}

		return bHandled;
	}

	bool SlotWidget::OnMouseButtonEvent(const MouseButtonEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseButtonEvent(MouseEv);

		if (Child)
		{
			bHandled |= Child->OnMouseButtonEvent(MouseEv);
		}

		return bHandled;
	}

	bool SlotWidget::OnMouseClicked(const MouseClickEvent& MouseEv)
	{
		if (Child)
		{
			return Child->OnMouseClicked(MouseEv);
		}

		return false;
	}

	bool SlotWidget::OnMouseDragged(const MouseDragEvent& MouseEv)
	{
		if (Child)
		{
			return Child->OnMouseDragged(MouseEv);
		}

		return false;
	}

	bool SlotWidget::OnMouseScroll(const MouseScrollEvent& MouseEv)
	{
		if (Child)
		{
			return Child->OnMouseScroll(MouseEv);
		}

		return false;
	}

	bool SlotWidget::OnChar(const CharEvent& CharEv)
	{
		if (Child)
		{
			return Child->OnChar(CharEv);
		}

		return false;
	}

	bool SlotWidget::OnKey(const KeyEvent& KeyEv)
	{
		if (Child)
		{
			return Child->OnKey(KeyEv);
		}

		return false;
	}

	Widget& SlotWidget::operator[](SharedPtr<Ry::Widget> Child)
	{
		SetChild(Child);

		return *this;
	}

	void SlotWidget::OnHovered(const MouseEvent& MouseEv)
	{
		Widget::OnHovered(MouseEv);
	}

	void SlotWidget::OnUnhovered(const MouseEvent& MouseEv)
	{
		Widget::OnUnhovered(MouseEv);
	}

	bool SlotWidget::OnPressed(const MouseButtonEvent& MouseEv)
	{
		Widget::OnPressed(MouseEv);

		return true;
	}

	bool SlotWidget::OnReleased(const MouseButtonEvent& MouseEv)
	{
		Widget::OnReleased(MouseEv);

		return true;
	}


}