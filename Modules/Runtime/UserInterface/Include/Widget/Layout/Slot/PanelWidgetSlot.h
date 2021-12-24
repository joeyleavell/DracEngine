#pragma once

#include "Widget/Widget.h"
#include "PanelWidgetSlot.gen.h"

namespace Ry
{

	const uint8 HOR_ALIGN_LEFT = 0;
	const uint8 HOR_ALIGN_CENTER = 1;
	const uint8 HOR_ALIGN_RIGHT = 2;
	const uint8 HOR_ALIGN_FILL = 3;

	const uint8 VERT_ALIGN_LEFT = 0;
	const uint8 VERT_ALIGN_CENTER = 1;
	const uint8 VERT_ALIGN_RIGHT = 2;
	const uint8 VERT_ALIGN_FILL = 3;

	class PanelWidgetSlot : public Ry::Object
	{
	public:

		GeneratedBody()

		// TODO: support reflected bools
		RefField()
		uint8 bSizeToContent{false};
		
		RefField()
		float SlotWidth{0.0f};

		RefField()
		float SlotHeight{0.0f};

		RefField()
		uint8 HorizontalAlignMode;

		RefField()
		uint8 VerticalAlignMode;

		//SizeMode Mode;
		//SizeType Size;

		RefField()
		float PaddingLeft{};

		RefField()
		float PaddingRight{};

		RefField()
		float PaddingTop{};

		RefField()
		float PaddingBottom{};


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
		//
		// Margin GetPadding()
		// {
		// 	return SlotPadding;
		// }

		PanelWidgetSlot& SetPadding(float Padding)
		{
			this->PaddingLeft = PaddingRight = PaddingTop = PaddingBottom = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetBottomPadding(float Padding)
		{
			this->PaddingBottom = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetTopPadding(float Padding)
		{
			this->PaddingTop = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetLeftPadding(float Padding)
		{
			this->PaddingLeft = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetRightPadding(float Padding)
		{
			this->PaddingRight = Padding;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetPadding(float Horizontal, float Vertical)
		{
			//this->SlotPadding.Set(Horizontal, Vertical);
			this->PaddingLeft = PaddingRight = Horizontal;
			this->PaddingTop = PaddingBottom = Vertical;

			MarkWidgetDirty();

			return *this;
		}

		PanelWidgetSlot& SetPadding(Margin Padding)
		{
			this->PaddingBottom = Padding.Bottom;
			this->PaddingTop = Padding.Top;
			this->PaddingLeft = Padding.Left;
			this->PaddingRight = Padding.Right;

			MarkWidgetDirty();

			return *this;
		}

	protected:

		SharedPtr<Ry::Widget> Widget;

		void MarkWidgetDirty()
		{
			if (Widget.IsValid())
			{
				Widget->RearrangeAndRepaint();
			}
		}

	} RefClass();
}
