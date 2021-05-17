#pragma once

#include "SlotWidget.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE Button : public SlotWidget
	{
	public:

		MulticastDelegate<> OnButtonPressed;
		MulticastDelegate<> OnButtonReleased;
		MulticastDelegate<> OnButtonHovered;

		void OnHovered(const MouseEvent& MouseEv) override
		{
			OnButtonHovered.Broadcast();
		}

		bool OnPressed(const MouseButtonEvent& MouseEv) override
		{
			if(IsHovered())
			{
				bButtonPressed = true;
				OnButtonPressed.Broadcast();
			}

			return true;
		}

		bool OnReleased(const MouseButtonEvent& MouseEv) override
		{
			if(bButtonPressed)
			{
				OnButtonReleased.Broadcast();
				bButtonPressed = false;
			}

			return true;
		}

	private:

		bool bButtonPressed = false;
		
	};
	
}
