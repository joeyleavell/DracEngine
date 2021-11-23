#include "Widget/Layout/Splitter.h"
#include "Buttons.h"

namespace Ry
{

	Splitter::Splitter()
	{
		SplitterItem = MakeItem();

		this->Type = SPLITTER_TYPE_HOR;
		this->BarThickness = 20.0f;
		this->BarPosition = 0.5f;
		this->MinBarPosition = 0.1;
		this->MaxBarPosition = 0.9;
	};

	Splitter::Slot Splitter::MakeSlot()
	{
		Splitter::Slot NewSlot;
		return NewSlot;
	}

	RectScissor Splitter::GetClipSpace(const Widget* ForWidget) const
	{
		// Default nullptr (entire widget) clip space
		if (!ForWidget)
			return Widget::GetClipSpace(ForWidget);

		RectScissor OurSize = Widget::GetClipSpace(this);
		//Point OurPos = GetAbsolutePosition();

		if (Type == SPLITTER_TYPE_HOR)
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
		return Widget::GetScaledSlotSize(this);
	}

	void Splitter::Arrange()
	{
		PanelWidget::Arrange();

		// Nothing to do
		if (Children.GetSize() <= 0)
			return;

		SharedPtr<Widget> WidgetA = Children[0];
		SharedPtr<Widget> WidgetB = Children[1];
		SharedPtr<PanelWidgetSlot> SlotA = WidgetSlots.Get(WidgetA.Get());
		SharedPtr<PanelWidgetSlot> SlotB = WidgetSlots.Get(WidgetB.Get());

		RectScissor OurSize = Widget::GetClipSpace(this);

		// Vertical or horizontal, widget A's position is always the same
		WidgetA->SetRelativePosition(SlotA->PaddingLeft, SlotA->PaddingBottom);

		// Arrange left to right
		if(Type == SPLITTER_TYPE_HOR)
		{
			float LeftWidth = OurSize.Width * BarPosition;
			WidgetB->SetRelativePosition(std::round(LeftWidth + BarThickness) + SlotB->PaddingLeft, SlotB->PaddingBottom);
		}
		else // Vertical
		{
			float BottomHeight = OurSize.Height * BarPosition;
			WidgetB->SetRelativePosition(SlotB->PaddingLeft, std::round(BottomHeight + BarThickness) + SlotB->PaddingBottom);
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
			if(Type == SPLITTER_TYPE_HOR)
			{
				float DeltaX = std::floor(MouseEv.MouseX - OurPos.X - BarThickness / 2.0f);
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
		if(Type == SPLITTER_TYPE_HOR)
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

	SizeType Splitter::GetScaledSlotSize(const Widget* ForWidget) const
	{
		// Result is the same padding or no padding due to clipping
		if (WidgetSlots.Contains(const_cast<Widget* const>(ForWidget)))
		{
			SizeType Size = ComputeSize();

			SharedPtr<PanelWidgetSlot> SlotA = WidgetSlots.Get(Children[0].Get());
			SharedPtr<PanelWidgetSlot> SlotB = WidgetSlots.Get(Children[1].Get());

			SizeType Result;

			if (Type == SPLITTER_TYPE_HOR)
			{
				// Left
				if (ForWidget == Children[0].Get())
				{
					Result = SizeType{ (int32)(BarPosition * Size.Width), Size.Height };
				}
				else // Right
				{
					Result = SizeType{ Size.Width - (int32)(BarPosition * Size.Width), Size.Height };
				}
			}
			else
			{
				// Bottom
				if (ForWidget == Children[0].Get())
				{
					Result = SizeType{ Size.Width, (int32)(BarPosition * Size.Height) };
				}
				else // Top
				{
					Result = SizeType{ Size.Width, Size.Height - (int32)(BarPosition * Size.Height) };
				}

			}

			if(ForWidget == Children[0].Get())
			{
				Result.Width  -= SlotA->PaddingLeft + SlotA->PaddingRight;
				Result.Height -= SlotA->PaddingTop + SlotA->PaddingBottom;
			}
			else
			{
				Result.Width -= SlotB->PaddingLeft + SlotB->PaddingRight;
				Result.Height -= SlotB->PaddingTop + SlotB->PaddingBottom;
			}

			return Result;
		}

		Ry::Log->LogErrorf("Error");

		return SizeType{ 0, 0 };
	}

	SizeType Splitter::GetUnscaledSlotSize(const Widget* ForWidget) const
	{
		return GetScaledSlotSize(ForWidget);
	}

	SharedPtr<PanelWidgetSlot> Splitter::AppendSlot(SharedPtr<Widget> Widget)
	{
		PanelWidget::AppendSlot(Widget);

		// Create widget
		SharedPtr<Slot> SplitterSlot = MakeShared(new Slot(Widget));

		WidgetSlots.Insert(Widget.Get(), SplitterSlot);

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
		if(Type == SPLITTER_TYPE_HOR)
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
