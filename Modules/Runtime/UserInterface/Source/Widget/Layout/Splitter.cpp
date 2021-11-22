#include "Widget/Layout/Splitter.h"
#include "Buttons.h"

namespace Ry
{

	Splitter::Splitter() {};

	Splitter::Slot Splitter::MakeSlot()
	{
		Splitter::Slot NewSlot;
		return NewSlot;
	}

	void Splitter::Construct(Splitter::Args& In)
	{
		CORE_ASSERT(In.Slots.GetSize() <= 2);

		this->Type = In.mType;
		this->BarThickness = In.mBarThickness;
		this->BarPosition = 0.5f;
		this->MinBarPosition = 0.1;
		this->MaxBarPosition = 0.9;

		for (PanelWidgetSlot& Child : In.Slots)
		{
			SharedPtr<Slot> Result = CastShared<Slot>(AppendSlot(Child.GetWidget()));
			Result->SetPadding(Child.GetPadding());
		}

		SplitterItem = MakeItem();
	}

	RectScissor Splitter::GetClipSpace(const Widget* ForWidget) const
	{
		// Default nullptr (entire widget) clip space
		if (!ForWidget)
			return Widget::GetClipSpace(ForWidget);

		RectScissor OurSize = Widget::GetClipSpace(this);
		//Point OurPos = GetAbsolutePosition();

		if (Type == SplitterType::HORIZONTAL)
		{
			int32 SplitterWidth = std::round(OurSize.Width * BarPosition);

			if (ForWidget == Children[0].Get())
			{
				// Clip space for left widget
				return RectScissor{ OurSize.X, OurSize.Y, SplitterWidth, OurSize.Height };
			}
			else
			{
				// Clip space for right widget
				return RectScissor{ (int32)std::round(OurSize.X + SplitterWidth + BarThickness), OurSize.Y, (int32)std::round(OurSize.Width - SplitterWidth - BarPosition), OurSize.Height };
			}
		}
		else // Vertical
		{
			float SplitterHeight = OurSize.Height * BarPosition;

			if (ForWidget == Children[0].Get())
			{
				// Clip space for bottom widget
				return RectScissor{ OurSize.X, OurSize.Y, OurSize.Width, (int32)std::round(SplitterHeight) };
			}
			else
			{
				// Clip space for top widget
				return RectScissor{ OurSize.X, (int32)std::round(OurSize.Y + SplitterHeight + BarThickness), OurSize.Width, (int32)std::round(OurSize.Height - SplitterHeight - BarThickness) };
			}

		}

	}

	void Splitter::GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates)
	{
		OutStates.Add(GetPipelineState(Children[0].Get()));
		OutStates.Add(GetPipelineState(Children[1].Get()));
		OutStates.Add(GetPipelineState(nullptr));
	}

	PipelineState Splitter::GetPipelineState(const Widget* ForWidget) const
	{
		// Default pipeline state
		if (!ForWidget)
			return Widget::GetPipelineState(ForWidget);

		PipelineState State;
		State.Scissor = GetClipSpace(ForWidget);

		// Must have two children
		if (ForWidget == Children[0].Get())
		{
			State.StateID = Ry::to_string(GetWidgetID()) + "_A";
		}
		else
		{
			State.StateID = Ry::to_string(GetWidgetID()) + "_B";
		}

		return State;
	}

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
		WidgetA->SetRelativePosition(0, 0.0f);

		// Arrange left to right
		if(Type == SplitterType::HORIZONTAL)
		{
			float LeftWidth = OurSize.Width * BarPosition;
			WidgetB->SetRelativePosition(std::round(LeftWidth + BarThickness), 0.0f);
		}
		else // Vertical
		{
			float BottomHeight = OurSize.Height * BarPosition;
			WidgetB->SetRelativePosition(0.0f, std::round(BottomHeight + BarThickness));
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
				float DeltaX = std::floor(MouseEv.MouseX - (OurPos.X - BarThickness / 2.0f));
				BarPosition = DeltaX / OurSize.Width;
			}
			else // Vertical
			{
				float DeltaY = std::floor(MouseEv.MouseY - OurPos.Y - BarThickness / 2.0f);
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
		if(Type == SplitterType::HORIZONTAL)
		{
			float PosX = std::ceil(OurPos.X + OurSize.Width * BarPosition);
			SplitterBarBounds = RectScissor{ (int32)PosX, OurPos.Y, (int32)BarThickness, OurSize.Height };
		}
		else // Vertical
		{
			float PosY = std::ceil(OurPos.Y + OurSize.Height * BarPosition);
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

	SharedPtr<PanelWidgetSlot> Splitter::AppendSlot(SharedPtr<Widget> Widget)
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

	void Splitter::Draw()
	{
		RectScissor OurSize = Widget::GetClipSpace(this);
		Point OurPos = GetAbsolutePosition();
		
		// Draw splitter bar
		if(Type == SplitterType::HORIZONTAL)
		{
			float PosX = OurPos.X + std::round(  OurSize.Width * BarPosition);
			Ry::BatchRectangle(SplitterItem, WHITE, PosX, OurPos.Y, BarThickness, OurSize.Height, 1.0f);
		}
		else // Vertical
		{
			int32 PosY = OurPos.Y + std::round(OurSize.Height * BarPosition);
			Ry::BatchRectangle(SplitterItem, WHITE, OurPos.X, PosY, OurSize.Width, BarThickness, 1.0f);
		}

		PanelWidget::Draw();
	}
	
}
