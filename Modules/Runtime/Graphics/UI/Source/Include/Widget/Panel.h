#pragma once

#include "Widget/Widget.h"
#include <vector>
#include "UIGen.h"
#include "Data/ArrayList.h"

namespace Ry
{

	struct Slot
	{
		Ry::Widget* Widget;
		float LeftMargin;
		float RightMargin;
		float TopMargin;
		float BottomMargin;

	};

	class UI_MODULE PanelWidget : public Widget
	{
	public:

		float SlotMargin;

		// PanelWidget(std::initializer_list<Ry::Widget&> Children):
		// SlotMargin(5.0f)
		// {
		// 	for(Ry::Widget& Widget : Children)
		// 	{
		// 		AppendSlot(Widget);
		// 	}
		// }

		void OnChildSizeDirty()
		{
			// Rearrange all widgets with respect to the panel
			Arrange();

			SizeDirty.Broadcast();
		}

		PanelWidget& SetMargins(float Margins)
		{
			this->SlotMargin = Margins;

			return *this;
		}

		SizeType ComputeSize() const override
		{
			SizeType Result;
			Result.Width = 0;
			Result.Height = 0;

			// Width is 2 * Margin + MaxChildWidth

			if (!PanelSlots.IsEmpty())
			{
				// Initial margins
				Result.Width = static_cast<int32>(2 * SlotMargin);
				Result.Height = static_cast<int32>(SlotMargin);

				int32 MaxChildWidth = 0;

				for (const Slot& Slot : PanelSlots)
				{
					SizeType WidgetSize = Slot.Widget->ComputeSize();

					if (WidgetSize.Width > MaxChildWidth)
					{
						MaxChildWidth = WidgetSize.Width;
					}

					Result.Height += static_cast<int32>(WidgetSize.Height + SlotMargin);
				}

				Result.Width += MaxChildWidth;
			}

			return Result;
		}

		void AppendSlot(Ry::Widget& Widget)
		{
			// Create widget
			Slot PanelSlot;
			PanelSlot.Widget = &Widget;

			// Listen for size changes
			Widget.SizeDirty.AddMemberFunction(this, &PanelWidget::OnChildSizeDirty);

			PanelSlots.Add(PanelSlot);

			// Set the widget's parent
			Widget.SetParent(this);
		}

		void Show() override
		{
			for (const Slot& Slot : PanelSlots)
			{
				Slot.Widget->Show();
			}
		}

		void Hide() override
		{
			for (const Slot& Slot : PanelSlots)
			{
				Slot.Widget->Hide();
			}
		}

		void Draw() override
		{
			for (const Slot& Slot : PanelSlots)
			{
				Slot.Widget->Draw();
			}
		}

		PanelWidget& operator+(Ry::Widget& Widget)
		{
			AppendSlot(Widget);

			return *this;
		}

	protected:

		Ry::ArrayList<Slot> PanelSlots;
	};

}
