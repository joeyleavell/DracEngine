#pragma once

#include "Widget/Widget.h"
#include "PanelWidgetSlot.gen.h"

namespace Ry
{
	class PanelWidgetSlot
	{
	public:

		GeneratedBody()

		SizeMode Mode;
		SizeType Size;

		PanelWidgetSlot()
		{
		}

		virtual ~PanelWidgetSlot()
		{

		}

		PanelWidgetSlot(SharedPtr<Ry::Widget> Wid)
		{
			this->Widget = Wid;
		}

		PanelWidgetSlot& operator[](SharedPtr<Ry::Widget> Child)
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

		PanelWidgetSlot& SetPadding(float Padding)
		{
			this->SlotPadding = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetBottomPadding(float Padding)
		{
			this->SlotPadding.Bottom = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetTopPadding(float Padding)
		{
			this->SlotPadding.Top = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetLeftPadding(float Padding)
		{
			this->SlotPadding.Left = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetRightPadding(float Padding)
		{
			this->SlotPadding.Right = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetPadding(float Horizontal, float Vertical)
		{
			this->SlotPadding.Set(Horizontal, Vertical);

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetPadding(Margin Padding)
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
			if (Widget.IsValid())
			{
				Widget->MarkDirty(Widget.Get());
			}
		}

	} RefClass();
}
