#pragma once

#include "Widget/Widget.h"
#include <vector>
#include "UIGen.h"
#include "Data/ArrayList.h"

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
					Widget->MarkDirty();
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
		
		void OnChildSizeDirty()
		{
			// Rearrange all widgets with respect to the panel
			Arrange();

			MarkDirty();
		}

		virtual SharedPtr<Slot> AppendSlot(SharedPtr<Widget> Widget)
		{
			// Listen for size changes
			Widget->RenderStateDirty.AddMemberFunction(this, &PanelWidget::OnChildSizeDirty);

			Children.Add(Widget);

			// Set existing batches
			// TODO: this should just be a pointer back up to user interface
			Widget->SetBatch(Bat);

			// Set the widget's parent
			Widget->SetParent(this);
			Widget->SetVisible(IsVisible(), true); // Child matches our visibility

			return SharedPtr<Slot>{nullptr};
		}

		virtual void OnShow() override
		{
		}

		virtual void OnHide() override
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

		virtual void Draw() override
		{
			RectScissor ClipSpace = GetClipSpace();

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
						Child->Arrange();
					}
					
					Child->Draw();
				}
				else
				{
					if (Child->IsVisible())
					{
						Child->SetVisible(false, true);
					}
				}
				
			}
		}

		PanelWidget& operator+(Ry::SharedPtr<Widget>& Widget)
		{
			AppendSlot(Widget);

			return *this;
		}

		void SetBatch(Batch* Bat) override
		{
			Widget::SetBatch(Bat);

			for(SharedPtr<Widget> Sl : Children)
			{
				Sl->SetBatch(Bat);
			}
		}

		void SetVisible(bool bVisibility, bool bPropagate) override
		{
			Widget::SetVisible(bVisibility, bPropagate);

			if (bPropagate)
			{
				for (SharedPtr<Widget> Child : Children)
				{
					Child->SetVisible(bVisibility, bPropagate);
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
