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

		// Arrange left to right
		if(Type == SplitterType::HORIZONTAL)
		{
			SizeType OurSize = ComputeSize();

			float LeftWidth = OurSize.Width * BarPosition;

			WidgetA->SetRelativePosition(0, 0);
			WidgetB->SetRelativePosition(LeftWidth + BarThickness / 2.0f, 0.0f);
		}

		WidgetA->Arrange();
		WidgetB->Arrange();
	}

	bool Splitter::OnMouseEvent(const MouseEvent& MouseEv)
	{
		if(bDragging)
		{
			SizeType OurSize = ComputeSize();
			Point OurPos = GetAbsolutePosition();
			float DeltaX = MouseEv.MouseX - OurPos.X;

			// Calculate new bar position
			BarPosition = DeltaX / OurSize.Width;
			BarPosition = std::max(0.0f, BarPosition);
			BarPosition = std::min(1.0f, BarPosition);

			MarkDirty(this);
			
			return true;
		}
		else
		{
			return PanelWidget::OnMouseEvent(MouseEv);
		}
	}

	bool Splitter::OnMouseButtonEvent(const MouseButtonEvent& MouseEv)
	{
		SizeType OurSize = ComputeSize();
		Point OurPos = GetAbsolutePosition();
		float PosX = OurPos.X + OurSize.Width * BarPosition - BarThickness / 2.0f;

		//Ry::BatchRectangle(SplitterItem, WHITE, PosX, OurPos.Y, BarThickness, OurSize.Height, 1.0f);
		RectScissor SplitterBarBounds{ (int32) PosX, OurPos.Y, (int32) BarThickness, OurSize.Height };

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

		Batch->AddItem(SplitterItem, "Shape", GetPipelineState(), nullptr, WidgetLayer + 1);
	}

	void Splitter::OnHide(Ry::Batch* Batch)
	{
		PanelWidget::OnHide(Batch);

		Batch->RemoveItem(SplitterItem);
	}

	void Splitter::Draw(StyleSet* Style)
	{

		// Draw splitter bar
		if(Type == SplitterType::HORIZONTAL)
		{
			SizeType OurSize = ComputeSize();
			Point OurPos = GetAbsolutePosition();
			float PosX = OurPos.X + OurSize.Width * BarPosition - BarThickness / 2.0f;

			Ry::BatchRectangle(SplitterItem, WHITE, PosX, OurPos.Y, BarThickness, OurSize.Height, 1.0f);
		}

		PanelWidget::Draw(Style);
	}
	
}
