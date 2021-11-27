#include "Widget/Layout/GridPanel.h"

namespace Ry
{

	bool GridPanel::IsSlotOccupied(int32 Row, int32 Col) const
	{
		SlotPosition Pos;
		Pos.Row = Row;
		Pos.Col = Col;

		return Occupied.Contains(Pos);
	}

	SlotPosition GridPanel::FindFree()
	{
		int Row = 0, Col = 0;
		int32 MaxCols = CalcMaxCols();

		// Set a 
		while (true)
		{

			while (Col < MaxCols)
			{

				if (!IsSlotOccupied(Row, Col))
				{
					SlotPosition Res;
					Res.Row = Row;
					Res.Col = Col;

					return Res;
				}

				Col++;
			}

			Col = 0;
			Row++;
		}


	}

	GridPanel::Slot GridPanel::MakeSlot(int32 Row, int32 Column)
	{
		Slot NewSlot;
		//NewSlot.Row = Row;
		//NewSlot.Column = Column;

		return NewSlot;
	}

	Ry::SharedPtr<PanelWidgetSlot> GridPanel::AppendSlot(Ry::SharedPtr<Widget> Widget)
	{
		// Create widget
		SharedPtr<GridPanel::Slot> PanelSlot = MakeShared(new GridPanel::Slot(Widget));

		// Find next available slot
		SlotPosition Next = FindFree();
		//PanelSlot->Row = Next.Row;
		//PanelSlot->Column = Next.Col;

		Occupied.Insert(Next);
		ChildrenSlots.Add(PanelSlot);

		WidgetSlots.Insert(Widget.Get(), PanelSlot);

		PanelWidget::AppendSlot(Widget);

		MarkDirty(this);

		return PanelSlot;
	}

	/**
	 * Arrange widgets vertically.
	 */
	void GridPanel::Arrange()
	{
		// Default margin: 5px

		// For each row, determine max width. That is the width of each cell.
		// Apply the same logic for height

		// Find max row/col
		int32 MaxRows = CalcMaxRows();
		int32 MaxCols = CalcMaxCols();
		//int32 CurRow = MaxRows - 1;

		SizeType ThisSize = Widget::GetScaledSlotSize(this);

		float CurrentY = static_cast<int32>(0.0f);

		// See how many columns will fit

		for(int32 SlotIndex = ChildrenSlots.GetSize() - 1; SlotIndex >=0; SlotIndex--)
		{
			SharedPtr<Slot>& CurSlot = ChildrenSlots[SlotIndex];

			int32 Row = CalcMaxRows() - SlotIndex / MaxCols - 1;
			int32 Col = SlotIndex % MaxCols;

			// Find the slot with this position
			/*Slot* FoundSlot = nullptr;
			for (SharedPtr<Slot>& Sl : ChildrenSlots)
			{
				if (Sl->Row == CurRow && Sl->Column == Col)
					FoundSlot = Sl.Get();
			}*/

			if (CurSlot.IsValid())
			{
				SharedPtr<Ry::Widget> Widget = CurSlot->GetWidget();
				SizeType WidgetSize = Widget->ComputeSize();

				// Add padding now
				//CurrentX += FoundSlot->GetPadding().Left;
				//CurrentY += FoundSlot->GetPadding().Top;

				//int32 CellMX = CurrentX + (int32)(CellSize / 2.0f);
				//int32 CellMY = CurrentY + (int32)(CellSize / 2.0f);

				//int32 WidgetX = CellMX - (int32)(ContentSize.Width / 2);
				//int32 WidgetY = CellMY - (int32)(ContentSize.Height / 2);

				float WidgetX = Col * CellWidth;
				float WidgetY = Row * CellHeight;

				// Place the widget in the middle of the cell

				// Set the widget's relative position
				Widget->SetRelativePosition(static_cast<float>(WidgetX + CurSlot->PaddingLeft), static_cast<float>(WidgetY + CurSlot->PaddingBottom));
				Widget->Arrange();

				//CurrentX += FoundSlot->PaddingRight;
				//CurrentY += FoundSlot->PaddingTop;
			}
		}

		/*while (CurRow >= 0)
		{

			float CurrentX = static_cast<int32>(0);

			for (int32 Col = 0; Col < MaxCols; Col++)
			{

				if (IsSlotOccupied(CurRow, Col))
				{


				}

				// Assume slot padding on left and right to be zero unless a slot is found
				CurrentX += CellWidth;

			}

			CurrentY += CellHeight;

			CurRow--;
		}*/

	}

	SizeType GridPanel::ComputeSize() const
	{
		//return Widget::GetScaledSlotSize(this);
		// Default margin: 5px

		int32 MaxCols = CalcMaxCols();

		int32 SizeX = static_cast<int32>(MaxCols * CellWidth);
		int32 SizeY = static_cast<int32>(0.0f);

		// Find max row/col
		int32 MaxRow = 0;
		bool bRowExists = true;

		while (bRowExists)
		{
			bRowExists = false;

			float CurrentX = 0.0f;

			for (int32 Col = 0; Col < MaxCols; Col++, CurrentX += CellWidth)
			{
				if (IsSlotOccupied(MaxRow, Col))
				{
					bRowExists = true;
				}
			}

			if (bRowExists)
			{
				SizeY += (int32)CellHeight;
			}

			MaxRow++;
		}

		return SizeType{ SizeX, SizeY };
	}

	GridPanel& GridPanel::SetCellWidth(float Size)
	{
		this->CellWidth = Size;

		return *this;
	}

	GridPanel& GridPanel::SetCellHeight(float Size)
	{
		this->CellHeight = Size;

		return *this;
	}

	SizeType GridPanel::GetScaledSlotSize(const Widget* ForWidget) const
	{
		// Result is the same including padding or not (for now)

		if (WidgetSlots.Contains(const_cast<Widget* const>(ForWidget)))
		{
			return SizeType{ (int32)CellWidth, (int32)CellHeight };
		}

		return SizeType{ 0, 0 };
	}

	SizeType GridPanel::GetUnscaledSlotSize(const Widget* ForWidget) const
	{
		return GetScaledSlotSize(ForWidget);
	}

	RectScissor GridPanel::GetClipSpace(const Widget* ForWidget) const
	{
		/*int32 Index = 0;
		for(SharedPtr<Slot> Slot : ChildrenSlots)
		{
			if(ForWidget == Slot->GetWidget().Get())
			{
				int32 MaxRows = CalcMaxRows();
				int32 MaxCols = CalcMaxCols();

				int32 Row = Index / MaxRows;
				int32 Col = Index % MaxCols;

				Point Abs = GetAbsolutePosition();
				int32 X = Abs.X + Col * CellWidth;
				int32 Y = Abs.Y + Row * CellHeight;
				return RectScissor{ X, Y, (int32) CellWidth, (int32)CellHeight };
			}
			Index++;
		}*/

		return Widget::GetClipSpace(this);
	}

	int32 GridPanel::CalcMaxRows() const
	{
		int32 MaxCols = CalcMaxCols();
		
		// Find max row/col
		int32 MaxRow = 0;
		bool bRowExists = true;

		while (bRowExists)
		{
			MaxRow++;
			bRowExists = false;

			for (int32 Col = 0; Col < MaxCols; Col++)
			{
				if (IsSlotOccupied(MaxRow, Col))
				{
					bRowExists = true;
					break;
				}
			}

		}

		return MaxRow;

	}

	int32 GridPanel::CalcMaxCols() const
	{
		SizeType ThisSize = Widget::GetScaledSlotSize(this);

		// See how many columns will fit
		return (int32) std::max(std::floor(ThisSize.Width / CellWidth), 1.0f);
	}

	void GridPanel::ClearChildren()
	{
		PanelWidget::ClearChildren();

		Occupied.Clear();
		ChildrenSlots.Clear();
	}

}