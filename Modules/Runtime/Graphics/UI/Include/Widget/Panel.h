#pragma once

#include "Widget/Widget.h"
#include <vector>
#include "UIGen.h"
#include "Data/ArrayList.h"
#include "2D/Batch/Batch.h"

namespace Ry
{

	class UI_MODULE PanelWidget : public Widget
	{
	public:

		class Slot
		{
		public:

			Slot()
			{
			}

			virtual ~Slot()
			{
				
			}

			Slot(SharedPtr<Ry::Widget> Wid)
			{
				this->Widget = Wid;
			}

			Slot& operator[](SharedPtr<Ry::Widget> Child)
			{
				this->Widget = Child;
				return *this;
			}

			SharedPtr<Ry::Widget> GetWidget()
			{
				return Widget;
			}

			Margin GetPadding()
			{
				return SlotPadding;
			}

			Slot& SetPadding(float Padding)
			{
				this->SlotPadding = Padding;

				MarkWidgetDirty();

				return *this;
			}

			Slot& SetBottomPadding(float Padding)
			{
				this->SlotPadding.Bottom = Padding;

				MarkWidgetDirty();

				return *this;
			}

			Slot& SetTopPadding(float Padding)
			{
				this->SlotPadding.Top = Padding;

				MarkWidgetDirty();

				return *this;
			}

			Slot& SetLeftPadding(float Padding)
			{
				this->SlotPadding.Left = Padding;

				MarkWidgetDirty();

				return *this;
			}

			Slot& SetRightPadding(float Padding)
			{
				this->SlotPadding.Right = Padding;

				MarkWidgetDirty();

				return *this;
			}

			Slot& SetPadding(float Horizontal, float Vertical)
			{
				this->SlotPadding.Set(Horizontal, Vertical);

				MarkWidgetDirty();

				return *this;
			}

			Slot& SetPadding(Margin Padding)
			{
				this->SlotPadding = Padding;

				MarkWidgetDirty();

				return *this;
			}

		protected:
			
			SharedPtr<Ry::Widget> Widget;
			Margin SlotPadding;

			void MarkWidgetDirty()
			{
				if(Widget.IsValid())
				{
					Widget->MarkDirty(Widget.Get());
				}
			}

		};

		WidgetBeginArgsSlot(PanelWidget)
		WidgetEndArgs()

		void Construct(Args& In)
		{
		}

		PanelWidget()
		{
			
		}

		virtual SharedPtr<Slot> AppendSlot(SharedPtr<Widget> Widget)
		{
			Children.Add(Widget);

			// Set the widget's parent
			Widget->SetParent(this);
			Widget->SetVisible(IsVisible(), true); // Child matches our visibility

			return SharedPtr<Slot>();
		}

		virtual void OnShow(Ry::Batch* Batch) override
		{
		}

		virtual void OnHide(Ry::Batch* Batch) override
		{
		}

		virtual void SetParent(Widget* Parent) override
		{
			Widget::SetParent(Parent);

			// Update child's depth
			for(SharedPtr<Widget> Child : Children)
			{
				Child->SetParent(this);
			}
		}

		void GetAllChildren(Ry::ArrayList<Widget*>& OutChildren) override
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

		virtual void Draw(StyleSet* Style) override
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
				if(!ClipSpace.IsEnabled() || ClipSpace.TestAgainst(WidgetBounds))
				{
					if(!Child->IsVisible())
					{
						Child->SetVisible(true, true);
						bUpdateBatch = true;
					}
					
					Child->Draw(Style);
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

			if(bUpdateBatch)
			{
				MarkDirty(this);
			}

		}

		PanelWidget& operator+(Ry::SharedPtr<Widget>& Widget)
		{
			AppendSlot(Widget);

			return *this;
		}

		void SetVisible(bool bVisibility, bool bPropagate) override
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

		bool OnMouseEvent(const MouseEvent& MouseEv) override
		{
			bool bHandled = Widget::OnMouseEvent(MouseEv);

			// This means we're hovering, pass to children
			for(SharedPtr<Widget> Child : Children)
			{
				Child->OnMouseEvent(MouseEv);
			}

			return bHandled;
		}

		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override
		{
			bool bHandled = Widget::OnMouseButtonEvent(MouseEv);

			for (SharedPtr<Widget> Child : Children)
			{
				bHandled |= Child->OnMouseButtonEvent(MouseEv);
			}

			return bHandled;
		}

		bool OnMouseClicked(const MouseClickEvent& MouseEv) override
		{
			bool bHandled = Widget::OnMouseClicked(MouseEv);

			for (SharedPtr<Widget> Child : Children)
			{
				bHandled |= Child->OnMouseClicked(MouseEv);
			}

			return bHandled;
		}

		bool OnMouseDragged(const MouseDragEvent& MouseEv) override
		{
			bool bHandled = Widget::OnMouseDragged(MouseEv);

			for (SharedPtr<Widget> Child : Children)
			{
				bHandled |= Child->OnMouseDragged(MouseEv);
			}

			return bHandled;
		}

		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override
		{
			bool bHandled = Widget::OnMouseScroll(MouseEv);

			for (SharedPtr<Widget> Child : Children)
			{
				bHandled |= Child->OnMouseScroll(MouseEv);
			}

			return bHandled;
		}

		bool OnKey(const KeyEvent& KeyEv) override
		{
			bool bHandled = Widget::OnKey(KeyEv);

			for (SharedPtr<Widget> Child : Children)
			{
				bHandled |= Child->OnKey(KeyEv);
			}

			return bHandled;
		}

		bool OnChar(const CharEvent& CharEv) override
		{
			bool bHandled = Widget::OnChar(CharEv);

			for (SharedPtr<Widget> Child : Children)
			{
				bHandled |= Child->OnChar(CharEv);
			}

			return bHandled;
		}

		virtual void ClearChildren()
		{
			for(SharedPtr<Widget> Child : Children)
			{
				Child->SetVisible(false, true);
			}
			
			Children.Clear();
		}

	protected:

		Ry::ArrayList<SharedPtr<Widget>> Children;
	};

}
