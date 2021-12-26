#include "Widget/Widget.h"
#include "Buttons.h"
#include "Core/Globals.h"

namespace Ry
{
	static uint32 IDCounter;

	Widget::Widget() :
		Parent(nullptr),
		WidgetLayer(0),
		RelativePosition{ 0, 0 },
		bHovered(false),
		bPressed(false),
		bVisible(true)
	{
		this->WidgetID = IDCounter++;
		this->Style = nullptr;
	}

	void Widget::SetId(const Ry::String& Id)
	{
		this->Id = Id;
	}

	void Widget::SetClass(const Ry::String& Class)
	{
		this->Class = Class;
	}

	void Widget::SetStyle(const Ry::StyleSet* Style)
	{
		this->Style = Style;
	}

	const Ry::StyleSet* Widget::GetStyle() const
	{
		return Style;
	}

	const Ry::String& Widget::GetId() const
	{
		return Id;
	}

	const Ry::String& Widget::GetClassName() const
	{
		return Class;
	}

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
		bool bCurrentlyPressed = false;
		if (MouseEv.ButtonID == MOUSE_BUTTON_LEFT && MouseEv.bPressed)
		{
			// OnPressed
			bCurrentlyPressed = true;
		}

		if (bCurrentlyPressed && !bPressed)
		{
			bPressed = true;
			OnPressed(MouseEv);
		}

		if (!bCurrentlyPressed && bPressed)
		{
			bPressed = false;
			OnReleased(MouseEv);
		}

		return false;
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

	bool Widget::OnPathDrop(const PathDropEvent& PathDropEv)
	{
		return false;
	}

	bool Widget::IsHovered()
	{
		return bHovered;
	}

	bool Widget::IsPressed()
	{
		return bPressed;
	}

	bool Widget::IsVisible() const
	{
		return bVisible;
	}

	Point Widget::GetRelativePosition() const
	{
		return RelativePosition;
	}

	Point Widget::GetAbsolutePosition() const
	{
		Point AbsolutePosition = GetRelativePosition();
		if (Parent)
		{
			Point ParentPos = Parent->GetAbsolutePosition();
			AbsolutePosition.X += ParentPos.X;
			AbsolutePosition.Y += ParentPos.Y;
		}

		return AbsolutePosition;
	}

	void Widget::SetRelativePosition(float X, float Y)
	{
		RelativePosition.X = (int32)X;
		RelativePosition.Y = (int32)Y;
	}

	void Widget::FullRefresh()
	{
		Ry::Widget* TopParent = FindTopParent();
		if (TopParent != this)
		{
			FindTopParent()->FullRefresh();
		}
	}

	void Widget::Rearrange(Widget* Widget)
	{
		// By default, arrange this widget
		if (!Widget)
			Widget = this;

		Ry::Widget* TopParent = FindTopParent();
		if(TopParent != this)
		{
			FindTopParent()->Rearrange(Widget);
		}

	}

	void Widget::UpdateBatch()
	{
		Ry::Widget* TopParent = FindTopParent();
		if (TopParent != this)
		{
			FindTopParent()->UpdateBatch();
		}
	}

	void Widget::SetParent(Widget* Parent)
	{
		this->Parent = Parent;

		// Calculate widget layer for new parent			
		WidgetLayer = 0;
		Widget* Temp = Parent;
		while (Temp)
		{
			WidgetLayer++;
			Temp = Temp->Parent;
		}

		Rearrange();
	}

	PipelineState Widget::GetPipelineState(const Widget* ForWidget) const
	{
		if (Parent)
		{
			// Return clip space of parent
			return Parent->GetPipelineState(this);
		}
		else
		{
			// By default widgets never clip
			PipelineState State;
			State.Scissor = GetClipSpace(ForWidget);
			State.StateID = Ry::to_string(WidgetID);

			return State;
		}
	}

	RectScissor Widget::GetClipSpace(const Widget* ForWidget) const
	{
		if (Parent)
		{
			// Return clip space of parent
			return Parent->GetClipSpace(this);
		}
		else
		{
			// By default widgets never clip
			RectScissor Clip{ 0, 0, Ry::GetViewportWidth(), Ry::GetViewportHeight() };
			return Clip;
		}
	}

	void Widget::Arrange()
	{
	}

	void Widget::OnHovered(const MouseEvent& MouseEv)
	{

	}

	void Widget::OnUnhovered(const MouseEvent& MouseEv)
	{

	}

	bool Widget::OnPressed(const MouseButtonEvent& MouseEv)
	{
		return false;
	}

	bool Widget::OnReleased(const MouseButtonEvent& MouseEv)
	{
		return false;
	}

	bool Widget::OnEvent(const Event& Ev)
	{
		if (Ev.Type == EVENT_MOUSE)
		{
			const MouseEvent& Mouse = static_cast<const MouseEvent&>(Ev);

			return OnMouseEvent(Mouse);
		}
		else if (Ev.Type == EVENT_MOUSE_BUTTON)
		{
			const MouseButtonEvent& MouseButton = static_cast<const MouseButtonEvent&>(Ev);

			return OnMouseButtonEvent(MouseButton);
		}
		else if (Ev.Type == EVENT_MOUSE_CLICK)
		{
			const MouseClickEvent& MouseClick = static_cast<const MouseClickEvent&>(Ev);
			return OnMouseClicked(MouseClick);
		}
		else if (Ev.Type == EVENT_MOUSE_DRAG)
		{
			const MouseDragEvent& MouseDrag = static_cast<const MouseDragEvent&>(Ev);
			return OnMouseDragged(MouseDrag);
		}
		else if (Ev.Type == EVENT_MOUSE_SCROLL)
		{
			const MouseScrollEvent& MouseScroll = static_cast<const MouseScrollEvent&>(Ev);
			return OnMouseScroll(MouseScroll);
		}
		else if (Ev.Type == EVENT_KEY)
		{
			const KeyEvent& Key = static_cast<const KeyEvent&>(Ev);
			return OnKey(Key);
		}
		else if (Ev.Type == EVENT_CHAR)
		{
			const CharEvent& Char = static_cast<const CharEvent&>(Ev);
			return OnChar(Char);
		}
		else if (Ev.Type == EVENT_PATH_DROP)
		{
			const PathDropEvent& PathDropEv = static_cast<const PathDropEvent&>(Ev);
			return OnPathDrop(PathDropEv);
		}

		return false;
	}

	void Widget::SetVisible(bool bVisibility, bool bPropagate)
	{
		// Sets the actual value of visibility and ensures propagation
		SetVisibleFlag(bVisibility, bPropagate);

		// Tells the batch to update since we likely added/removed geometry during this visibility change.
		UpdateBatch();
	}

	void Widget::SetVisibleFlag(bool bVisibility, bool bPropagate)
	{
		this->bVisible = bVisibility;
	}

	void Widget::GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates, bool bRecurse)
	{
		// By default, add the pipeline state associated with the entire widget.
		// Children widgets can have their own dynamic pipeline states, but most widgets don't implement this.
		// One widget that does implement this is the splitter widget.

		//OutStates.Add(GetPipelineState(nullptr));
	}

	void Widget::SetBatch(Batch* Bat)
	{
		this->Bat = Bat;
	}

	Batch* Widget::GetBatch()
	{
		Widget* TopMostParent = this;
		while(TopMostParent && TopMostParent->Parent)
		{
			TopMostParent = TopMostParent->Parent;
		}

		return TopMostParent->Bat;
	}

	SizeType Widget::GetScaledSlotSize(const Widget* ForWidget) const
	{
		if (!Parent)
		{
			return SizeType{ Ry::GetViewportWidth(), Ry::GetViewportHeight() };
		}
		else
		{
			return Parent->GetScaledSlotSize(this);
		}
	}

	SizeType Widget::GetUnscaledSlotSize(const Widget* ForWidget) const
	{
		if (!Parent)
		{
			return SizeType{ Ry::GetViewportWidth(), Ry::GetViewportHeight() };
		}
		else
		{
			return Parent->GetUnscaledSlotSize(this);
		}
	}

	SizeType Widget::GetScaledOccupiedSize(const Widget* ForWidget) const
	{
		if(!Parent)
		{
			return SizeType{ Ry::GetViewportWidth(), Ry::GetViewportHeight() };
		}
		else
		{
			return Parent->GetScaledOccupiedSize(this);
		}
	}

	SizeType Widget::GetUnscaledOccupiedSize(const Widget* ForWidget) const
	{
		if (!Parent)
		{
			return SizeType{ Ry::GetViewportWidth(), Ry::GetViewportHeight() };
		}
		else
		{
			return Parent->GetUnscaledOccupiedSize(this);
		}
	}


	Widget* Widget::FindTopParent()
	{
		Widget* CurrentParent = this;
		while (CurrentParent && CurrentParent->Parent)
		{
			CurrentParent = CurrentParent->Parent;
		}
		return CurrentParent;
	}

	int32 Widget::GetWidgetID() const
	{
		return WidgetID;
	}


}
