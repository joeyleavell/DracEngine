#pragma once

#include "Widget/Panel.h"
#include "2D/Batch/Batch.h"

namespace Ry
{

	enum class SplitterType
	{
		VERTICAL,
		HORIZONTAL
	};

    class UI_MODULE Splitter : public Ry::PanelWidget
    {
    public:

        SplitterType Type;
		float BarThickness;

    	/*
    	 * Determines what percentage the bar is along at. In the range [0, 1].
    	 */
		float BarPosition;

		WidgetBeginArgsSlot(Splitter)
			WidgetProp(SplitterType, Type)
			WidgetProp(float, BarThickness)
		WidgetEndArgs()

		Splitter() {};

		struct Slot : public PanelWidget::Slot
		{
			Slot() :
				PanelWidget::Slot()
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid) :
				PanelWidget::Slot(Wid)
			{
			}
		};

		static Splitter::Slot MakeSlot()
		{
			Splitter::Slot NewSlot;
			return NewSlot;
		}

		void Construct(Splitter::Args& In)
		{
			CORE_ASSERT(In.Slots.GetSize() <= 2);

			this->Type = In.mType;
			this->BarThickness = In.mBarThickness;
			this->BarPosition = 0.5f;

			for (PanelWidget::Slot& Child : In.Slots)
			{
				SharedPtr<Slot> Result = CastShared<Slot>(AppendSlot(Child.GetWidget()));
				Result->SetPadding(Child.GetPadding());
			}

			SplitterItem = MakeItem();
		}

    	SharedPtr<PanelWidget::Slot> AppendSlot(SharedPtr<Widget> Widget) override;

    	void OnShow(Ry::Batch* Batch) override;
    	void OnHide(Ry::Batch* Batch) override;

    	SizeType ComputeSize() const override;
        void Arrange() override;
		void Draw(StyleSet* Style) override;

    	bool OnMouseEvent(const MouseEvent& MouseEv) override;
    	bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;

    private:

		bool bDragging = false;

		Ry::SharedPtr<Ry::BatchItem> SplitterItem;


    };

}