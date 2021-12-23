#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "Splitter.gen.h"

namespace Ry
{

	const uint8 SPLITTER_TYPE_VERT = 0;
	const uint8 SPLITTER_TYPE_HOR = 1;

    class USERINTERFACE_MODULE Splitter : public Ry::PanelWidget
    {
    public:

    	GeneratedBody()

    	RefField()
        uint8 Type;

    	RefField()
    	float BarThickness;
    	
		float MinBarPosition;
		float MaxBarPosition;

    	/*
    	 * Determines what percentage the bar is along at. In the range [0, 1].
    	 */
		float BarPosition;

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

    	SizeType GetScaledSlotSize(const Widget* ForWidget) const override;
    	SizeType GetUnscaledSlotSize(const Widget* ForWidget) const override;
    	
    private:

		bool bDragging = false;

		Ry::SharedPtr<Ry::BatchItem> SplitterItem;


    } RefClass();

}