#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "2D/Batch/Batch.h"
#include "Drawable.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE ScrollPane : public PanelWidget
	{
	public:

		struct Slot : public PanelWidget::Slot
		{
			Slot() :
			PanelWidget::Slot()
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid):
			PanelWidget::Slot(Wid)
			{
			}

			ScrollPane::Slot& operator[](SharedPtr<Ry::Widget> Child)
			{
				this->Widget = Child;
				return *this;
			}

		};

		WidgetBeginArgsSlot(ScrollPane)
			WidgetProp(int32, Width)
			WidgetProp(int32, Height)
		WidgetEndArgs()

		void Construct(Args& In);
		float GetVerticalScrollAmount();
		float GetHorizontalScrollAmount();
		void VerticalScroll(float Pixels);
		void HorizontalScroll(float Pixels);
		void SetHorizontalScrollAmount(float Scroll);
		void SetVerticalScrollAmount(float Scroll);
		static ScrollPane::Slot MakeSlot();
		SharedPtr<PanelWidget::Slot> AppendSlot(SharedPtr<Widget> Widget) override;
		void Draw() override;
		/**
		 * Arrange widgets vertically.
		 */
		void Arrange() override;
		PipelineState GetPipelineState(const Widget* ForWidget) const override;
		RectScissor GetClipSpace(const Widget* ForWidget) const override;
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		SizeType ComputeSize() const override;
		void ClearChildren() override;
		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override;
		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;
		bool OnMouseEvent(const MouseEvent& MouseEv) override;

	private:

		void GetVertScrollBarBounds(Point& OutPos, SizeType& OutSize);
		void GetHorScrollBarBounds(Point& OutPos, SizeType& OutSize);
		// Utility to compute the size of the children embedded within the scroll pane
		SizeType ComputeChildrenSize() const;

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;
		SizeType Size;

		Ry::SharedPtr<BatchItem> DebugRect;

		float VerticalScrollAmount;
		float HorizontalScrollAmount;

		float ScrollBarThickness;

		Ry::SharedPtr<BoxDrawable> VerticalScrollBar;
		Ry::SharedPtr<BoxDrawable> HorizontalScrollBar;

		bool bVerticalBarPressed;
		bool bHorizontalBarPressed;
		Point RelativeScrollBarPressed;

		bool bAllowHorizontalScroll;

		RectScissor LastClip;
	};

}
