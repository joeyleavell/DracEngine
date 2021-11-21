#include "Widget/Layout/GridPanel.h"

namespace Ry
{

	void GridPanel::Construct(Args& In)
	{
		PanelWidget::Args ParentArgs;
		PanelWidget::Construct(ParentArgs);

		SetCellWidth(In.mCellWidth);
		SetCellHeight(In.mCellHeight);

		for (Ry::SharedPtr<Widget>& Child : In.Children)
		{
			AppendSlot(Child);
		}
	}

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
		NewSlot.Row = Row;
		NewSlot.Column = Column;

		return NewSlot;
	}

	Ry::SharedPtr<PanelWidget::Slot> GridPanel::AppendSlot(Ry::SharedPtr<Widget> Widget)
	{
		// Create widget
		SharedPtr<GridPanel::Slot> PanelSlot = MakeShared(new GridPanel::Slot(Widget));

		// Find next available slot
		SlotPosition Next = FindFree();
		PanelSlot->Row = Next.Row;
		PanelSlot->Column = Next.Col;

		Occupied.Insert(Next);
		ChildrenSlots.Add(PanelSlot);

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
		float CurrentY = static_cast<int32>(0);

		// For each row, determine max width. That is the width of each cell.
		// Apply the same logic for height

		// Find max row/col
		int32 MaxRow = 0;
		bool bRowExists = true;

		while (bRowExists)
		{
			bRowExists = false;

			float CurrentX = static_cast<int32>(0);

			for (int32 Col = 0; Col < MaxCols; Col++)
			{

				if (IsSlotOccupied(MaxRow, Col))
				{
					bRowExists = true;

					// Find the slot with this position
					Slot* FoundSlot = nullptr;
					for (SharedPtr<Slot>& Sl : ChildrenSlots)
					{
						if (Sl->Row == MaxRow && Sl->Column == Col)
							FoundSlot = Sl.Get();
					}

					if (FoundSlot)
					{
						SharedPtr<Ry::Widget> Widget = FoundSlot->GetWidget();

						// Add padding now
						//CurrentX += FoundSlot->GetPadding().Left;
						//CurrentY += FoundSlot->GetPadding().Top;

						//int32 CellMX = CurrentX + (int32)(CellSize / 2.0f);
						//int32 CellMY = CurrentY + (int32)(CellSize / 2.0f);

						//int32 WidgetX = CellMX - (int32)(ContentSize.Width / 2);
						//int32 WidgetY = CellMY - (int32)(ContentSize.Height / 2);

						float WidgetX = CurrentX;
						float WidgetY = CurrentY;

						// Place the widget in the middle of the cell

						// Set the widget's relative position
						Widget->SetRelativePosition(static_cast<float>(WidgetX + FoundSlot->GetPadding().Left), static_cast<float>(WidgetY + FoundSlot->GetPadding().Bottom));
						Widget->Arrange();

						CurrentX += FoundSlot->GetPadding().Right;
						CurrentY += FoundSlot->GetPadding().Top;
					}

					bRowExists = true;
				}

				// Assume slot padding on left and right to be zero unless a slot is found
				CurrentX += CellWidth;

			}

			CurrentY += CellHeight;

			MaxRow++;
		}

	}

	SizeType GridPanel::ComputeSize() const
	{
		// Default margin: 5px
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

	GridPanel& GridPanel::SetMaxCols(int32 Max)
	{
		this->MaxCols = Max;

		return *this;
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

	void GridPanel::ClearChildren()
	{
		PanelWidget::ClearChildren();

		Occupied.Clear();
		ChildrenSlots.Clear();
	}

}