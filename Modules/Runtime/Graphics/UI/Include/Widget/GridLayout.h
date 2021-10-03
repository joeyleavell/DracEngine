#pragma once

#include "Widget/Panel.h"
#include "UIGen.h"
#include <chrono>
#include "Data/Set.h"

namespace Ry
{

	struct SlotPosition
	{
		int Row;
		int Col;

		bool operator==(const SlotPosition& Other) const
		{
			return Row == Other.Row && Col == Other.Col;
		}
	};

	class UI_MODULE GridLayout : public PanelWidget
	{
	public:
		
		class Slot : public PanelWidget::Slot
		{
		public:
			int32 Row;
			int32 Column;

			Slot() :
			PanelWidget::Slot(),
			Row(0),
			Column(0)
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid) :
			PanelWidget::Slot(Wid),
			Row(0),
			Column(0)
			{
			}
			
		};
		
		WidgetBeginArgsSlot(GridLayout)
			WidgetProp(float, CellWidth)
			WidgetProp(float, CellHeight)
		WidgetEndArgs()

		void Construct(Args& In)
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


	private:

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

		static Slot MakeSlot(int32 Row, int32 Column)
		{
			Slot NewSlot;
			NewSlot.Row = Row;
			NewSlot.Column = Column;
			
			return NewSlot;
		}

		virtual Ry::SharedPtr<PanelWidget::Slot> AppendSlot(Ry::SharedPtr<Widget> Widget) override
		{
			PanelWidget::AppendSlot(Widget);

			// Create widget
			SharedPtr<GridLayout::Slot> PanelSlot = MakeShared(new GridLayout::Slot(Widget));

			// Find next available slot
			SlotPosition Next = FindFree();
			PanelSlot->Row = Next.Row;
			PanelSlot->Column = Next.Col;

			Occupied.Insert(Next);
			ChildrenSlots.Add(PanelSlot);

			return PanelSlot;
		}
		
		/**
		 * Arrange widgets vertically.
		 */
		virtual void Arrange() override
		{
			// Default margin: 5px
			float CurrentY = static_cast<int32>(0);

			// For each row, determine max width. That is the width of each cell.
			// Apply the same logic for height

			// Find max row/col
			int32 MaxRow = 0;
			bool bRowExists = true;
			
			while(bRowExists)
			{
				bRowExists = false;

				float CurrentX = static_cast<int32>(0);

				for (int32 Col = 0; Col < MaxCols; Col++)
				{
					
					if(IsSlotOccupied(MaxRow, Col))
					{
						bRowExists = true;

						// Find the slot with this position
						Slot* FoundSlot = nullptr;
						for(SharedPtr<Slot>& Sl : ChildrenSlots)
						{
							if (Sl->Row == MaxRow && Sl->Column == Col)
								FoundSlot = Sl.Get();
						}

						if(FoundSlot)
						{
							SharedPtr<Ry::Widget> Widget = FoundSlot->GetWidget();
							SizeType ContentSize = Widget->ComputeSize();

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

		SizeType ComputeSize() const override
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

				if(bRowExists)
				{
					SizeY += (int32) CellHeight;
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

		void ClearChildren() override
		{
			PanelWidget::ClearChildren();
			
			Occupied.Clear();
			ChildrenSlots.Clear();
		}

		
	private:

		int32 MaxCols = 5;

		float CellWidth  = 50.0f;
		float CellHeight = 50.0f;

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

		// TODO: implement this as a set
		Ry::Set<SlotPosition> Occupied;

	};

	template <>
	inline uint32 HashImpl<Ry::SlotPosition>(TypeTag<Ry::SlotPosition>, const Ry::SlotPosition& Object)
	{
		uint32 Hash = 17;
		Hash = Hash * 37 + Object.Col;
		Hash = Hash * 37 + Object.Row;
		return Hash;
	}


}
