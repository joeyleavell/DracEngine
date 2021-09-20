#pragma once

#include "SlotWidget.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE Button : public SlotWidget
	{
	public:

		WidgetBeginArgs(Button)
			WidgetProp(float, FillX)
			WidgetProp(float, FillY)
			WidgetPropDefault(float, Padding, 0.0f)
			WidgetPropDefault(VAlign, VerticalAlignment, VAlign::CENTER)
			WidgetPropDefault(HAlign, HorizontalAlignment, HAlign::CENTER)
		WidgetEndArgs()

		void Construct(Args& In)
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

		MulticastDelegate<> OnButtonPressed;
		MulticastDelegate<> OnButtonReleased;
		MulticastDelegate<> OnButtonHovered;

		void OnHovered(const MouseEvent& MouseEv) override
		{
			OnButtonHovered.Broadcast();
		}

		bool OnPressed(const MouseButtonEvent& MouseEv) override
		{
			if(IsHovered())
			{
				bButtonPressed = true;
				OnButtonPressed.Broadcast();
			}

			return true;
		}

		bool OnReleased(const MouseButtonEvent& MouseEv) override
		{
			if(bButtonPressed)
			{
				OnButtonReleased.Broadcast();
				bButtonPressed = false;
			}

			return true;
		}

	private:

		bool bButtonPressed = false;
		
	};
	
}
