#include "Widget/Layout/SlotWidget.h"
#include "Core/Globals.h"

namespace Ry
{

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

		this->WidthMode = SIZE_MODE_AUTO;
		this->HeightMode = SIZE_MODE_AUTO;

		this->VerticalAlign = VERT_TOP_ALIGN;
		this->HorizontalAlign = HOR_LEFT_ALIGN;
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

	SlotWidget& SlotWidget::AutoWidth()
	{
		this->WidthMode = SIZE_MODE_AUTO;

		return *this;
	}

	SlotWidget& SlotWidget::AutoHeight()
	{
		this->WidthMode = SIZE_MODE_AUTO;

		return *this;
	}

	SizeType SlotWidget::ComputeSize() const
	{
		SizeType ParentSize;
		SizeType ChildSize;
		SizeType Result;

		// Compute child size, only if needed otherwise we'd enter infinite recursion
		if (Child && (WidthMode == SIZE_MODE_AUTO || HeightMode == SIZE_MODE_AUTO))
		{
			ChildSize = Child->ComputeSize();
		}

		// Compute parent size only if we need to
		if ((WidthMode == SIZE_MODE_PERCENTAGE || HeightMode == SIZE_MODE_PERCENTAGE))
		{
			SizeType SlotSize = Widget::GetScaledSlotSize(this);
			ParentSize.Width = SlotSize.Width;
			ParentSize.Height = SlotSize.Height;
		}

		// else
		// {
		// }

		if (WidthMode == SIZE_MODE_PERCENTAGE)
		{
			Result.Width = (int32)(ParentSize.Width * FillX);
		}
		else if (WidthMode == SIZE_MODE_AUTO)
		{
			Result.Width = (int32)(PaddingLeft + PaddingRight + ChildSize.Width);
		}

		if (HeightMode == SIZE_MODE_PERCENTAGE)
		{
			Result.Height = (int32)(ParentSize.Height * FillY);
		}
		else if (HeightMode == SIZE_MODE_AUTO)
		{
			Result.Height = (int32)(PaddingTop + PaddingBottom + ChildSize.Height);
		}

		return Result;
	}

	void SlotWidget::SetVisibleFlag(bool bVisibility, bool bPropagate)
	{
		Widget::SetVisibleFlag(bVisibility, bPropagate);
		
		if (Child && bPropagate)
		{
			Child->SetVisibleFlag(bVisibility, true);
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

	void SlotWidget::Update()
	{
		Widget::Update();
		if(Child)
		{
			Child->Update();
		}
	}

	void SlotWidget::Arrange()
	{
		if (Child)
		{
			SizeType ChildSize = Child->ComputeSize();
			float RelX = PaddingLeft;
			float RelY = PaddingBottom;

			if (WidthMode == SIZE_MODE_PERCENTAGE)
			{
				RectScissor ThisSize = Widget::GetClipSpace(this);
				switch ((uint32)HorizontalAlign)
				{
				case (uint32)HOR_CENTER_ALIGN:
					RelX = (ThisSize.Width - ChildSize.Width) / 2.0f;
					break;
				case (uint32)HOR_LEFT_ALIGN:
					RelX = PaddingLeft;
					break;
				case (uint32)HOR_RIGHT_ALIGN:
					RelX = (float)(ThisSize.Width - ChildSize.Width - PaddingRight);
					break;
				}
			}

			if (HeightMode == SIZE_MODE_PERCENTAGE)
			{
				RectScissor ThisSize = Widget::GetClipSpace(this);
				switch ((uint32)VerticalAlign)
				{
				case (uint32)VERT_CENTER_ALIGN:
					RelY = (ThisSize.Height - ChildSize.Height) / 2.0f;
					break;
				case (uint32)VERT_TOP_ALIGN:
					RelY = ThisSize.Height - ChildSize.Height - PaddingTop;
					break;
				case (uint32)VERT_BOTTOM_ALIGN:
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
		Child->SetParent(this);
		Child->SetVisible(IsVisible(), true); // Child matches our visibility
		Child->SetStyle(Style);
		if(Style)
		{
			Child->OnShow(GetBatch());
		}

		// Automatically rearrange
		Arrange();

		// Recompute cached size
		FullRefresh();
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

	bool SlotWidget::OnPathDrop(const PathDropEvent& PathDropEv)
	{
		if (Child)
		{
			return Child->OnPathDrop(PathDropEv);
		}

		return false;
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

	void SlotWidget::OnShow(Ry::Batch* Batch)
	{
		Widget::OnShow(Batch);
		if(Child)
		{
			Child->OnShow(Batch);
		}
	}

	void SlotWidget::OnHide(Ry::Batch* Batch)
	{
		Widget::OnHide(Batch);
		if (Child)
		{
			Child->OnHide(Batch);
		}
	}

	void SlotWidget::GetPipelineStates(Ry::ArrayList<PipelineState>& PipelineStates, bool bRecurse)
	{
		Widget::GetPipelineStates(PipelineStates, bRecurse);

		if(Child && bRecurse)
		{
			Child->GetPipelineStates(PipelineStates, true);
		}
	}

	SizeType SlotWidget::GetScaledOccupiedSize(const Widget* ForWidget) const
	{
		// Padding is guaranteed
		SizeType SlotSize = GetScaledSlotSize(ForWidget);
		SlotSize.Width += static_cast<int32>(PaddingLeft + PaddingRight);
		SlotSize.Height += static_cast<int32>(PaddingTop + PaddingBottom);

		return SlotSize;
	}

	SizeType SlotWidget::GetScaledSlotSize(const Widget* ForWidget) const
	{
		SizeType Unscaled = GetUnscaledOccupiedSize(ForWidget);
		SizeType Ours = Widget::GetScaledSlotSize(this);
		
		if (Unscaled.Width > Ours.Width)
			Unscaled.Width = static_cast<int32>(static_cast<float>(Ours.Width) - PaddingLeft - PaddingRight);
		if (Unscaled.Height > Ours.Height)
			Unscaled.Height = static_cast<int32>(static_cast<float>(Ours.Height) - PaddingTop - PaddingBottom);

		return Unscaled;
	}

	SizeType SlotWidget::GetUnscaledSlotSize(const Widget* ForWidget) const
	{
		if (ForWidget == Child.Get())
		{
			int32 Width;
			int32 Height;
			SizeType ChildSize;
			SizeType ParentSize;

			if (Child && (WidthMode == SIZE_MODE_AUTO || HeightMode == SIZE_MODE_AUTO))
				ChildSize = Child->ComputeSize();

			if (WidthMode == SIZE_MODE_PERCENTAGE || HeightMode == SIZE_MODE_PERCENTAGE)
			{
				ParentSize = Widget::GetScaledSlotSize(this); // Occupy a percentage of the size fully available to us
			}

			if (WidthMode == SIZE_MODE_AUTO)
			{
				Width = ChildSize.Width;
			}
			else
			{
				Width = (int32)(ParentSize.Width * FillX);
			}

			if (HeightMode == SIZE_MODE_AUTO)
			{
				Height = ChildSize.Height;
			}
			else
			{
				Height = (int32)(ParentSize.Height * FillY);
			}

			return SizeType{ Width, Height };
		}

		Ry::Log->LogError("Tried calling SlotWidget::GetSlotSize() with widget that is not this SlotWidget's child!");
		return SizeType{};
	}

	SizeType SlotWidget::GetUnscaledOccupiedSize(const Widget* ForWidget) const
	{
		SizeType SlotSize = GetUnscaledSlotSize(ForWidget);
		SlotSize.Width += static_cast<int32>(PaddingLeft + PaddingRight);
		SlotSize.Height += static_cast<int32>(PaddingTop + PaddingBottom);

		return SlotSize;
	}

}
