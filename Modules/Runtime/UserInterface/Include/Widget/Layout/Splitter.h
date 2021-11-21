#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "2D/Batch/Batch.h"

namespace Ry
{

	enum class SplitterType
	{
		VERTICAL,
		HORIZONTAL
	};

    class USERINTERFACE_MODULE Splitter : public Ry::PanelWidget
    {
    public:

        SplitterType Type;
		float BarThickness;
		float MinBarPosition;
		float MaxBarPosition;

    	/*
    	 * Determines what percentage the bar is along at. In the range [0, 1].
    	 */
		float BarPosition;

		WidgetBeginArgsSlot(Splitter)
			WidgetProp(SplitterType, Type)
			WidgetProp(float, BarThickness)
		WidgetEndArgs()

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

		static Splitter::Slot MakeSlot();

		Splitter();

		void Construct(Splitter::Args& In);
    	RectScissor GetClipSpace(const Widget* ForWidget) const override;
		void GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates) override;
		PipelineState GetPipelineState(const Widget* ForWidget) const override;
    	SharedPtr<PanelWidget::Slot> AppendSlot(SharedPtr<Widget> Widget) override;

    	void OnShow(Ry::Batch* Batch) override;
    	void OnHide(Ry::Batch* Batch) override;

    	SizeType ComputeSize() const override;
        void Arrange() override;
		void Draw() override;

    	bool OnMouseEvent(const MouseEvent& MouseEv) override;
    	bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;

    private:

		bool bDragging = false;

		Ry::SharedPtr<Ry::BatchItem> SplitterItem;


    };

}