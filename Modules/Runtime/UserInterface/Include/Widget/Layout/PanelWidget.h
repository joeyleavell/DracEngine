#pragma once

#include "Widget/Widget.h"
#include <vector>
#include "Data/ArrayList.h"
#include "2D/Batch/Batch.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE PanelWidget : public Widget
	{
	public:

		class Slot
		{
		public:

			SizeMode Mode;
			SizeType Size;

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

		PanelWidget();

		void Construct(Args& In);
		void SetStyle(const Ry::StyleSet* Style) override;
		virtual SharedPtr<Slot> AppendSlot(SharedPtr<Widget> Widget);
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void SetParent(Widget* Parent) override;
		void GetAllChildren(Ry::ArrayList<Widget*>& OutChildren) override;
		void Draw() override;
		PanelWidget& operator+(Ry::SharedPtr<Widget>& Widget);
		void SetVisible(bool bVisibility, bool bPropagate) override;
		bool OnMouseEvent(const MouseEvent& MouseEv) override;
		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;
		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;
		bool OnMouseDragged(const MouseDragEvent& MouseEv) override;
		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override;
		bool OnKey(const KeyEvent& KeyEv) override;
		bool OnChar(const CharEvent& CharEv) override;
		virtual void ClearChildren();

	protected:

		Ry::ArrayList<SharedPtr<Widget>> Children;
	};

}
