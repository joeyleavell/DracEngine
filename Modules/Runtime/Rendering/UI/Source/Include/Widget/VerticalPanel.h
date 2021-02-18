#pragma once

#include "Widget/Panel.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE VerticalLayout : public PanelWidget
	{
	public:
		
		/**
		 * Arrange widgets vertically.
		 */
		virtual void Arrange() override
		{
			// Default margin: 5px
			int32 CurrentX = static_cast<int32>(SlotMargin);
			int32 CurrentY = static_cast<int32>(SlotMargin);

			for (const Slot& Slot : PanelSlots)
			{
				Ry::Widget* Widget = Slot.Widget;
				SizeType ContentSize = Widget->ComputeSize();

				// Set the widget's relative position
				Widget->SetRelativePosition(static_cast<float>(CurrentX), static_cast<float>(CurrentY));

				CurrentY += static_cast<int32>(ContentSize.Height + SlotMargin);
			}
		}

	};

}
