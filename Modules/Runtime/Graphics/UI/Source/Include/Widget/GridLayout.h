#pragma once

#include "Widget/Panel.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE GridLayout : public PanelWidget
	{
	private:
		struct SlotPosition
		{
			int Row;
			int Col;

			bool operator==(const SlotPosition& Other)
			{
				return Row == Other.Row && Col == Other.Col;
			}
	
		};

		bool IsSlotOccupied(int32 Row, int32 Col) const
		{
			SlotPosition Pos;
			Pos.Row = Row;
			Pos.Col = Col;

			return Occupied.Contains(Pos);
		}

		SlotPosition FindFree()
		{
			int Row = 0, Col = 0;

			// Set a 
			while(true)
			{

				while(Col < MaxCols)
				{

					if(!IsSlotOccupied(Row, Col))
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
		
	public:

		struct Slot
		{
			Ry::Widget* Widget;
			int32 Row;
			int32 Column;
			
			float LeftMargin;
			float RightMargin;
			float TopMargin;
			float BottomMargin;
		};

		void AppendSlot(Ry::Widget& Widget) override
		{
			PanelWidget::AppendSlot(Widget);

			// Create widget
			Slot PanelSlot;
			PanelSlot.Widget = &Widget;

			// Find next available slot
			SlotPosition Next = FindFree();
			PanelSlot.Row = Next.Row;
			PanelSlot.Column = Next.Col;

			Occupied.Add(Next);

			ChildrenSlots.Add(PanelSlot);
		}
		
		/**
		 * Arrange widgets vertically.
		 */
		virtual void Arrange() override
		{
			// Default margin: 5px
			int32 CurrentY = static_cast<int32>(SlotMargin);

			// For each row, determine max width. That is the width of each cell.
			// Apply the same logic for height

			// Find max row/col
			int32 MaxRow = 0;
			bool bRowExists = true;
			
			while(bRowExists)
			{
				bRowExists = false;

				int32 CurrentX = static_cast<int32>(SlotMargin);

				for (int32 Col = 0; Col < MaxCols; Col++, CurrentX += CellWidth + SlotMargin)
				{
					if(IsSlotOccupied(MaxRow, Col))
					{
						bRowExists = true;

						// Find the slot with this position
						Slot* FoundSlot = nullptr;
						for(Slot& Sl : ChildrenSlots)
						{
							if (Sl.Row == MaxRow && Sl.Column == Col)
								FoundSlot = &Sl;
						}

						if(FoundSlot)
						{
							Ry::Widget* Widget = FoundSlot->Widget;
							SizeType ContentSize = Widget->ComputeSize();

							//int32 CellMX = CurrentX + (int32)(CellSize / 2.0f);
							//int32 CellMY = CurrentY + (int32)(CellSize / 2.0f);

							//int32 WidgetX = CellMX - (int32)(ContentSize.Width / 2);
							//int32 WidgetY = CellMY - (int32)(ContentSize.Height / 2);

							int32 WidgetX = CurrentX;
							int32 WidgetY = CurrentY;
							
							// Place the widget in the middle of the cell

							// Set the widget's relative position
							Widget->SetRelativePosition(static_cast<float>(WidgetX), static_cast<float>(WidgetY));
							Widget->Arrange();
						}

						bRowExists = true;
					}

				}

				CurrentY += CellHeight + SlotMargin;
				
				MaxRow++;
			}
			
		}

		SizeType ComputeSize() const override
		{
			// Default margin: 5px
			int32 SizeX = static_cast<int32>(2 * SlotMargin + MaxCols * CellWidth);
			int32 SizeY = static_cast<int32>(SlotMargin);

			// Find max row/col
			int32 MaxRow = 0;
			bool bRowExists = true;
			
			while (bRowExists)
			{
				bRowExists = false;

				int32 CurrentX = static_cast<int32>(SlotMargin);

				for (int32 Col = 0; Col < MaxCols; Col++, CurrentX += CellWidth + SlotMargin)
				{
					if (IsSlotOccupied(MaxRow, Col))
					{
						bRowExists = true;
					}
				}

				if(bRowExists)
				{
					SizeY += CellHeight + SlotMargin;
				}

				MaxRow++;
			}

			return SizeType{ SizeX, SizeY };
		}

		GridLayout& SetMaxCols(int32 Max)
		{
			this->MaxCols = Max;

			return *this;
		}

		GridLayout& SetCellWidth(float Size)
		{
			this->CellWidth = Size;

			return *this;
		}

		GridLayout& SetCellHeight(float Size)
		{
			this->CellHeight = Size;

			return *this;
		}

		
	private:

		int32 MaxCols = 5;

		float CellWidth= 50.0f;
		float CellHeight = 50.0f;

		Ry::ArrayList<Slot> ChildrenSlots;

		// TODO: implement this as a set
		Ry::ArrayList<SlotPosition> Occupied;

	};

}
