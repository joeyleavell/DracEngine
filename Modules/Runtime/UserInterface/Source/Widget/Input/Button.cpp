#include "Widget/Input/Button.h"

namespace Ry
{

	void Button::OnHovered(const MouseEvent& MouseEv)
	{
		OnButtonHovered.Broadcast();
	}

	bool Button::OnPressed(const MouseButtonEvent& MouseEv)
	{
		if (IsHovered())
		{
			bButtonPressed = true;
			OnButtonPressed.Broadcast();
		}

		return true;
	}

	bool Button::OnReleased(const MouseButtonEvent& MouseEv)
	{
		if (bButtonPressed)
		{
			OnButtonReleased.Broadcast();
			bButtonPressed = false;
		}

		return true;
	}

}
