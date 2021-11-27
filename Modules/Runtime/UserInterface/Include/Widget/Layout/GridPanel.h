#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "Data/Set.h"
#include "GridPanel.gen.h"

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

		GeneratedBody()

		RefField()
		float CellWidth = 50.0f;

		RefField()
		float CellHeight = 50.0f;

		class Slot : public PanelWidgetSlot
		{
		public:
			int32 Row;
			int32 Column;

			Slot() :
			PanelWidgetSlot(),
			Row(0),
			Column(0)
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid) :
			PanelWidgetSlot(Wid),
			Row(0),
			Column(0)
			{
			}
			
		};
		
		Ry::SharedPtr<PanelWidgetSlot> AppendSlot(Ry::SharedPtr<Widget> Widget) override;

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

		SizeType GetScaledSlotSize(const Widget* ForWidget) const override;
		SizeType GetUnscaledSlotSize(const Widget* ForWidget) const override;

	private:

		int32 CalcMaxRows();

		bool IsSlotOccupied(int32 Row, int32 Col) const;
		SlotPosition FindFree();

		int32 MaxCols = 5;

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

		Ry::Set<SlotPosition> Occupied;
	} RefClass();

	template <>
	inline uint32 HashImpl<Ry::SlotPosition>(HashTypeTag<Ry::SlotPosition>, const Ry::SlotPosition& Object)
	{
		uint32 Hash = 17;
		Hash = Hash * 37 + Object.Col;
		Hash = Hash * 37 + Object.Row;
		return Hash;
	}


}
