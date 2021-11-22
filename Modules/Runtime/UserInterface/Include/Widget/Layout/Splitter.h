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

		WidgetBeginArgsSlot(Splitter, PanelWidgetSlot)
			WidgetProp(SplitterType, Type)
			WidgetProp(float, BarThickness)
		WidgetEndArgs()

		struct Slot : public PanelWidgetSlot
		{
			Slot() :
				PanelWidgetSlot()
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid) :
				PanelWidgetSlot(Wid)
			{
			}
		};

		static Splitter::Slot MakeSlot();

		Splitter();

		void Construct(Splitter::Args& In);
    	RectScissor GetClipSpace(const Widget* ForWidget) const override;
		void GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates) override;
		PipelineState GetPipelineState(const Widget* ForWidget) const override;
    	SharedPtr<PanelWidgetSlot> AppendSlot(SharedPtr<Widget> Widget) override;

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