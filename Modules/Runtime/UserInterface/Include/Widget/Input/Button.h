#pragma once

#include "Widget/Layout/SlotWidget.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE Button : public SlotWidget
	{
	public:

		MulticastDelegate<> OnButtonPressed;
		MulticastDelegate<> OnButtonReleased;
		MulticastDelegate<> OnButtonHovered;

		WidgetBeginArgs(Button)
			WidgetProp(float, FillX)
			WidgetProp(float, FillY)
			WidgetPropDefault(float, Padding, 0.0f)
			WidgetPropDefault(VAlign, VerticalAlignment, VAlign::CENTER)
			WidgetPropDefault(HAlign, HorizontalAlignment, HAlign::CENTER)
		WidgetEndArgs()

		void Construct(Args& In);
		void OnHovered(const MouseEvent& MouseEv) override;
		bool OnPressed(const MouseButtonEvent& MouseEv) override;
		bool OnReleased(const MouseButtonEvent& MouseEv) override;

	private:

		bool bButtonPressed = false;
		
	};
	
}
