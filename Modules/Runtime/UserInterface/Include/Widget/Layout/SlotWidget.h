#pragma once

#include "Widget/Widget.h"
#include "Drawable.h"
#include "Style.h"
#include "SlotWidget.gen.h"

namespace Ry
{
	class USERINTERFACE_MODULE SlotWidget : public Widget
	{
	public:

		GeneratedBody()

		/**
		 * The padding of the inner content of this box element.
		 */
		RefField()
		float PaddingLeft;

		RefField()
		float PaddingRight;

		RefField()
		float PaddingTop;

		RefField()
		float PaddingBottom;

		// Percentage parameters
		RefField()
		float FillX;

		RefField()
		float FillY;

		RefField()
		uint8 VerticalAlign;

		RefField()
		uint8 HorizontalAlign;

		RefField()
		uint8 WidthMode;

		RefField()
		uint8 HeightMode;

		SlotWidget();

		SharedPtr<Widget> FindChildWidgetById(const Ry::String& Id) const override;
		void SetStyle(const Ry::StyleSet* Style) override;
		void SetParent(Widget* Parent) override;
		SlotWidget& Padding(float Pad);
		SlotWidget& Padding(float Vertical, float Horizontal);
		SlotWidget& Padding(float Left, float Right, float Top, float Bottom);
		SlotWidget& AutoWidth();
		SlotWidget& AutoHeight();
		SlotWidget& FillParent();
		SizeType ComputeSize() const override;
		void SetVisibleFlag(bool bVisibility, bool bPropagate) override;
		void Draw() override;
		void Update() override;
		void Arrange() override;
		void SetChild(Ry::SharedPtr<Ry::Widget> Child);
		bool OnMouseEvent(const MouseEvent& MouseEv) override;
		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;
		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;
		bool OnMouseDragged(const MouseDragEvent& MouseEv) override;
		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override;
		bool OnChar(const CharEvent& CharEv) override;
		bool OnKey(const KeyEvent& KeyEv) override;
		bool OnPathDrop(const PathDropEvent& PathDropEv) override;
		void OnHovered(const MouseEvent& MouseEv) override;
		void OnUnhovered(const MouseEvent& MouseEv) override;
		bool OnPressed(const MouseButtonEvent& MouseEv) override;
		bool OnReleased(const MouseButtonEvent& MouseEv) override;
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates, bool bRecurse) override;
		
		SizeType GetScaledOccupiedSize(const Widget* ForWidget) const override;
		SizeType GetScaledSlotSize(const Widget* ForWidget) const override;
		SizeType GetUnscaledOccupiedSize(const Widget* ForWidget) const override;
		SizeType GetUnscaledSlotSize(const Widget* ForWidget) const override;

	private:

		/**
		 * The child stored within this box element.
		 */
		SharedPtr<Ry::Widget> Child;
	} RefClass();
}
