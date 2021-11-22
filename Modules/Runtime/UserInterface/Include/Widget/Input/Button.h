#pragma once

#include "Widget/Layout/SlotWidget.h"
#include "Button.gen.h"

namespace Ry
{

	class USERINTERFACE_MODULE Button : public SlotWidget
	{
	public:

		GeneratedBody()

		MulticastDelegate<> OnButtonPressed;
		MulticastDelegate<> OnButtonReleased;
		MulticastDelegate<> OnButtonHovered;

		void OnHovered(const MouseEvent& MouseEv) override;
		bool OnPressed(const MouseButtonEvent& MouseEv) override;
		bool OnReleased(const MouseButtonEvent& MouseEv) override;

	private:

		bool bButtonPressed = false;
		
	} RefClass();
	
}
