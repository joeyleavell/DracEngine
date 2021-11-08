#include "Widget/Splitter.h"
#include "Buttons.h"

namespace Ry
{

	SizeType Splitter::ComputeSize() const
	{
		SizeType ParentSize;

		if (Parent)
		{
			ParentSize = Parent->ComputeSize();
		}
		else
		{
			ParentSize.Width = Ry::GetViewportWidth();
			ParentSize.Height = Ry::GetViewportHeight();
		}

		return ParentSize;
	}

	void Splitter::Arrange()
	{
		PanelWidget::Arrange();

		// Nothing to do
		if (Children.GetSize() <= 0)
			return;

		SharedPtr<Widget> WidgetA = Children[0];
		SharedPtr<Widget> WidgetB = Children[1];
		RectScissor OurSize = Widget::GetClipSpace(this);

		// Vertical or horizontal, widget A's position is always the same
		WidgetA->SetRelativePosition(0, 0);

		// Arrange left to right
		if(Type == SplitterType::HORIZONTAL)
		{
			float LeftWidth = OurSize.Width * BarPosition;
			WidgetB->SetRelativePosition(LeftWidth + BarThickness / 2.0f, 0.0f);
		}
		else // Vertical
		{
			float BottomHeight = OurSize.Height * BarPosition;
			WidgetB->SetRelativePosition(0.0f, BottomHeight + BarThickness / 2.0f);
		}

		WidgetA->Arrange();
		WidgetB->Arrange();
	}

	bool Splitter::OnMouseEvent(const MouseEvent& MouseEv)
	{
		if(bDragging)
		{
			RectScissor OurSize = Widget::GetClipSpace(this);
			Point OurPos = GetAbsolutePosition();

			// Calculate new bar position
			if(Type == SplitterType::HORIZONTAL)
			{
				float DeltaX = MouseEv.MouseX - OurPos.X;
				BarPosition = DeltaX / OurSize.Width;
			}
			else // Vertical
			{
				float DeltaY = MouseEv.MouseY - OurPos.Y;
				BarPosition = DeltaY / OurSize.Height;
			}

			BarPosition = std::max(MinBarPosition, BarPosition);
			BarPosition = std::min(MaxBarPosition, BarPosition);

			MarkDirty(this, true);
			
			return true;
		}
		else
		{
			return PanelWidget::OnMouseEvent(MouseEv);
		}
	}

	bool Splitter::OnMouseButtonEvent(const MouseButtonEvent& MouseEv)
	{
		RectScissor OurSize = Widget::GetClipSpace(this);
		Point OurPos = GetAbsolutePosition();

		RectScissor SplitterBarBounds;
		if(Type ==SplitterType::HORIZONTAL)
		{
			float PosX = OurPos.X + OurSize.Width * BarPosition - BarThickness / 2.0f;
			SplitterBarBounds = RectScissor{ (int32)PosX, OurPos.Y, (int32)BarThickness, OurSize.Height };
		}
		else // Vertical
		{
			float PosY = OurPos.Y + OurSize.Height * BarPosition - BarThickness / 2.0f;
			SplitterBarBounds = RectScissor{ (int32)OurPos.Y, (int32)PosY, OurSize.Width, (int32) BarThickness};
		}

		//Ry::BatchRectangle(SplitterItem, WHITE, PosX, OurPos.Y, BarThickness, OurSize.Height, 1.0f);

		bool bHandledHere = false;
		if(MouseEv.ButtonID == MOUSE_BUTTON_LEFT)
		{
			// Check for begin drag
			if(MouseEv.bPressed && !bDragging && SplitterBarBounds.TestAgainst(MouseEv.MouseX, MouseEv.MouseY))
			{
				bDragging = true;
				bHandledHere = true;
			}

			// Check for end drag
			if(bDragging && !MouseEv.bPressed)
			{
				bDragging = false;
				bHandledHere = true;
			}

		}

		if(!bHandledHere)
		{
			return PanelWidget::OnMouseButtonEvent(MouseEv);
		}
		else
		{
			return true;
		}
	}

	SharedPtr<PanelWidget::Slot> Splitter::AppendSlot(SharedPtr<Widget> Widget)
	{
		PanelWidget::AppendSlot(Widget);

		// Create widget
		SharedPtr<Slot> SplitterSlot = MakeShared(new Slot(Widget));
		return SplitterSlot;
	}

	void Splitter::OnShow(Ry::Batch* Batch)
	{
		PanelWidget::OnShow(Batch);

		Batch->AddItem(SplitterItem, "Shape", GetPipelineState(nullptr), nullptr, WidgetLayer + 1);
	}

	void Splitter::OnHide(Ry::Batch* Batch)
	{
		PanelWidget::OnHide(Batch);

		Batch->RemoveItem(SplitterItem);
	}

	void Splitter::Draw(StyleSet* Style)
	{
		RectScissor OurSize = Widget::GetClipSpace(this);
		Point OurPos = GetAbsolutePosition();
		
		// Draw splitter bar
		if(Type == SplitterType::HORIZONTAL)
		{
			float PosX = OurPos.X + OurSize.Width * BarPosition - BarThickness / 2.0f;
			Ry::BatchRectangle(SplitterItem, WHITE, PosX, OurPos.Y, BarThickness, OurSize.Height, 1.0f);
		}
		else // Vertical
		{
			float PosY = OurPos.Y + OurSize.Height * BarPosition - BarThickness / 2.0f;
			Ry::BatchRectangle(SplitterItem, WHITE, OurPos.X, PosY, OurSize.Width, BarThickness, 1.0f);
		}

		PanelWidget::Draw(Style);
	}
	
}
