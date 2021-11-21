#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "Data/Set.h"
#include "UserInterfaceGen.h"

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

	class USERINTERFACE_MODULE GridPanel : public PanelWidget
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
		
		WidgetBeginArgsSlot(GridPanel)
			WidgetProp(float, CellWidth)
			WidgetProp(float, CellHeight)
		WidgetEndArgs()

		void Construct(Args& In);

		Ry::SharedPtr<PanelWidget::Slot> AppendSlot(Ry::SharedPtr<Widget> Widget) override;

		/**
		 * Arrange widgets vertically.
		 */
		void Arrange() override;
		SizeType ComputeSize() const override;
		void ClearChildren() override;
		
		static Slot MakeSlot(int32 Row, int32 Column);

		GridPanel& SetMaxCols(int32 Max);
		GridPanel& SetCellWidth(float Size);
		GridPanel& SetCellHeight(float Size);

	private:

		bool IsSlotOccupied(int32 Row, int32 Col) const;
		SlotPosition FindFree();

		int32 MaxCols = 5;

		float CellWidth  = 50.0f;
		float CellHeight = 50.0f;

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

		Ry::Set<SlotPosition> Occupied;
	};

	template <>
	inline uint32 HashImpl<Ry::SlotPosition>(HashTypeTag<Ry::SlotPosition>, const Ry::SlotPosition& Object)
	{
		uint32 Hash = 17;
		Hash = Hash * 37 + Object.Col;
		Hash = Hash * 37 + Object.Row;
		return Hash;
	}


}
