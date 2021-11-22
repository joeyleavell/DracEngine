#include "Widget/Layout/PanelWidget.h"

namespace Ry
{
	
	PanelWidget::PanelWidget()
	{

	}

	SharedPtr<Widget> PanelWidget::FindChildWidgetById(const Ry::String& Id) const
	{
		for (SharedPtr<Widget> Child : Children)
		{
			if(Child->GetId() == Id)
			{
				return Child;
			}
			else
			{
				SharedPtr<Widget> ChildWidget = Child->FindChildWidgetById(Id);
				if(ChildWidget.IsValid())
				{
					return ChildWidget;
				}
			}
		}

		return SharedPtr<Widget>();
	}

	void PanelWidget::SetStyle(const Ry::StyleSet* Style)
	{
		Widget::SetStyle(Style);

		for(SharedPtr<Widget> Child : Children)
		{
			Child->SetStyle(Style);
		}
	}

	SharedPtr<PanelWidgetSlot> PanelWidget::AppendSlot(SharedPtr<Widget> Widget)
	{
		Children.Add(Widget);

		// Set the widget's parent
		Widget->SetStyle(Style);
		Widget->SetParent(this);
		Widget->SetVisible(IsVisible(), true); // Child matches our visibility
		
		return SharedPtr<PanelWidgetSlot>();
	}

	void PanelWidget::OnShow(Ry::Batch* Batch)
	{
	}

	void PanelWidget::OnHide(Ry::Batch* Batch)
	{
	}

	void PanelWidget::SetParent(Widget* Parent)
	{
		Widget::SetParent(Parent);

		// Update child's depth
		for (SharedPtr<Widget> Child : Children)
		{
			Child->SetParent(this);
		}
	}

	void PanelWidget::GetAllChildren(Ry::ArrayList<Widget*>& OutChildren)
	{
		for (SharedPtr<Widget> Child : Children)
		{
			if (Child.IsValid())
			{
				OutChildren.Add(Child.Get());
				Child->GetAllChildren(OutChildren);
			}
		}

	}

	void PanelWidget::Draw()
	{
		// Get clip space for entire widget
		RectScissor ClipSpace = GetClipSpace(nullptr);

		bool bUpdateBatch = false;

		for (SharedPtr<Widget> Child : Children)
		{
			Point Pos = Child->GetAbsolutePosition();
			SizeType ContentSize = Child->ComputeSize();

			RectScissor WidgetBounds{ Pos.X, Pos.Y, ContentSize.Width, ContentSize.Height };

			// Do cull test
			if (!ClipSpace.IsEnabled() || ClipSpace.TestAgainst(WidgetBounds))
			{
				if (!Child->IsVisible())
				{
					Child->SetVisible(true, true);
					bUpdateBatch = true;
				}

				Child->Draw();
			}
			else
			{
				if (Child->IsVisible())
				{
					Child->SetVisible(false, true);
					bUpdateBatch = true;
				}
			}

		}

		if (bUpdateBatch)
		{
			MarkDirty(this);
		}

	}

	PanelWidget& PanelWidget::operator+(Ry::SharedPtr<Widget>& Widget)
	{
		AppendSlot(Widget);

		return *this;
	}

	void PanelWidget::SetVisible(bool bVisibility, bool bPropagate)
	{
		Widget::SetVisible(bVisibility, bPropagate);

		if (bPropagate)
		{
			for (SharedPtr<Widget> Child : Children)
			{
				// Set child to its own visibility, needs to refresh
				Child->SetVisible(bVisibility, true);
			}
		}
	}

	bool PanelWidget::OnMouseEvent(const MouseEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseEvent(MouseEv);

		// This means we're hovering, pass to children
		for (SharedPtr<Widget> Child : Children)
		{
			Child->OnMouseEvent(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnMouseButtonEvent(const MouseButtonEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseButtonEvent(MouseEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnMouseButtonEvent(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnMouseClicked(const MouseClickEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseClicked(MouseEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnMouseClicked(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnMouseDragged(const MouseDragEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseDragged(MouseEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnMouseDragged(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnMouseScroll(const MouseScrollEvent& MouseEv)
	{
		bool bHandled = Widget::OnMouseScroll(MouseEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnMouseScroll(MouseEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnKey(const KeyEvent& KeyEv)
	{
		bool bHandled = Widget::OnKey(KeyEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnKey(KeyEv);
		}

		return bHandled;
	}

	bool PanelWidget::OnChar(const CharEvent& CharEv)
	{
		bool bHandled = Widget::OnChar(CharEv);

		for (SharedPtr<Widget> Child : Children)
		{
			bHandled |= Child->OnChar(CharEv);
		}

		return bHandled;
	}

	void PanelWidget::ClearChildren()
	{
		for (SharedPtr<Widget> Child : Children)
		{
			Child->SetVisible(false, true);
		}

		Children.Clear();
	}

}