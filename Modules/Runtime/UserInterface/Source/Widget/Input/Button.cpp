#include "Widget/Input/Button.h"

namespace Ry
{

	void Button::Construct(Args& In)
	{
		SlotWidget::Args ParentArgs;
		ParentArgs.mPadding = In.mPadding;
		ParentArgs.mVerticalAlignment = In.mVerticalAlignment;
		ParentArgs.mHorizontalAlignment = In.mHorizontalAlignment;
		ParentArgs.mFillX = In.mFillX;
		ParentArgs.mFillY = In.mFillY;
		ParentArgs.Children = In.Children;
		SlotWidget::Construct(ParentArgs);
	}

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
