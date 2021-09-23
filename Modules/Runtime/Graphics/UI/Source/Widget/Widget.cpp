#include "Widget/Widget.h"
#include "Buttons.h"

namespace Ry
{
	static uint32 IDCounter;

	Widget::Widget() :
		RelativePosition{ 0, 0 },
		MaxSize{ -1, -1 },
		Parent(nullptr),
		Bat(nullptr),
		bPressed(false),
		bHovered(false),
		bVisible(true),
		WidgetLayer(0)
	{
		this->WidgetID = IDCounter++;
	};

	bool Widget::OnMouseEvent(const MouseEvent& MouseEv)
	{
		// Check if we're inside the widget space
		Point Abs = GetAbsolutePosition();
		SizeType Size = ComputeSize(); // hopefully this is up to date?

		int32 MouseX = (int32)MouseEv.MouseX;
		int32 MouseY = (int32)MouseEv.MouseY;

		bool bIsCurrentlyHovering = false;

		if (MouseX >= Abs.X && MouseX < Abs.X + Size.Width)
		{
			if (MouseY >= Abs.Y && MouseY < Abs.Y + Size.Height)
			{
				bIsCurrentlyHovering = true;
			}
		}

		if (!bHovered && bIsCurrentlyHovering)
		{
			bHovered = true;
			OnHovered(MouseEv);
		}

		if (bHovered && !bIsCurrentlyHovering)
		{
			bHovered = false;
			OnUnhovered(MouseEv);
		}

		return bIsCurrentlyHovering;
	}

	bool Widget::OnMouseButtonEvent(const MouseButtonEvent& MouseEv)
	{
		bool bHandled = false;

		bool bCurrentlyPressed = false;
		if (MouseEv.ButtonID == MOUSE_BUTTON_LEFT && MouseEv.bPressed)
		{
			// OnPressed
			bCurrentlyPressed = true;
		}

		if (bCurrentlyPressed && !bPressed)
		{
			bPressed = true;
			bHandled = OnPressed(MouseEv);
		}

		if (!bCurrentlyPressed && bPressed)
		{
			bPressed = false;
			bHandled = OnReleased(MouseEv);
		}

		return bHandled;
	}

	bool Widget::OnMouseClicked(const MouseClickEvent& MouseEv)
	{
		return false;
	}

	bool Widget::OnMouseDragged(const MouseDragEvent& MouseEv)
	{
		return false;
	}

	bool Widget::OnMouseScroll(const MouseScrollEvent& MouseEv)
	{
		return false;
	}

	bool Widget::OnKey(const KeyEvent& KeyEv)
	{
		return false;
	}

	bool Widget::OnChar(const CharEvent& CharEv)
	{
		return false;
	}
}
