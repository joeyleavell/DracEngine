#pragma once

#include "Widget/Widget.h"
#include "Drawable.h"
#include "Style.h"
#include "UserInterfaceGen.h"

namespace Ry
{
	class USERINTERFACE_MODULE SlotWidget : public Widget
	{
	public:

		WidgetBeginArgs(SlotWidget)
			WidgetProp(float, FillX)
			WidgetProp(float, FillY)
			WidgetPropDefault(float, Padding, 0.0f)
			WidgetPropDefault(VAlign, VerticalAlignment, VAlign::CENTER)
			WidgetPropDefault(HAlign, HorizontalAlignment, HAlign::CENTER)
		WidgetEndArgs()

		SlotWidget();
		void Construct(Args& In);

		void SetParent(Widget* Parent) override;
		SlotWidget& Padding(float Pad);
		SlotWidget& Padding(float Vertical, float Horizontal);
		SlotWidget& Padding(float Left, float Right, float Top, float Bottom);
		SlotWidget& SetVerticalAlignment(VAlign Alignment);
		SlotWidget& SetHorizontalAlignment(HAlign Alignment);
		SlotWidget& AutoWidth();
		SlotWidget& AutoHeight();
		SlotWidget& FillX(float FillX);
		SlotWidget& FillY(float FillY);
		SlotWidget& FillParent();
		SlotWidget& SetVAlign(VAlign VerticalAlign);
		SlotWidget& SetHAlign(HAlign HorizontalAlign);
		void GetAllChildren(Ry::ArrayList<Widget*>& OutChildren) override;
		SizeType ComputeSize() const override;
		void SetVisible(bool bVisibility, bool bPropagate) override;
		void Draw(StyleSet* Style) override;
		void Arrange() override;
		void SetChild(Ry::SharedPtr<Ry::Widget> Child);
		bool OnMouseEvent(const MouseEvent& MouseEv) override;
		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;
		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;
		bool OnMouseDragged(const MouseDragEvent& MouseEv) override;
		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override;
		bool OnChar(const CharEvent& CharEv) override;
		bool OnKey(const KeyEvent& KeyEv) override;
		Widget& operator[](SharedPtr<Ry::Widget> Child) override;
		void OnHovered(const MouseEvent& MouseEv) override;
		void OnUnhovered(const MouseEvent& MouseEv) override;
		bool OnPressed(const MouseButtonEvent& MouseEv) override;
		bool OnReleased(const MouseButtonEvent& MouseEv) override;

	private:

		VAlign ContentVAlign;
		HAlign ContentHAlign;
		SizeMode WidthMode;
		SizeMode HeightMode;

		// Percentage parameters
		float FillXPercent;
		float FillYPercent;

		/**
		 * The child stored within this box element.
		 */
		SharedPtr<Ry::Widget> Child;

		/**
		 * The padding of the inner content of this box element.
		 */
		float PaddingLeft, PaddingRight, PaddingTop, PaddingBottom;

		VAlign VerticalAlign;
		HAlign HorizontalAlign;
	};
}
