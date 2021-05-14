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

		virtual void AppendSlot(Ry::Widget& Widget)
		{
			// Listen for size changes
			Widget.SizeDirty.AddMemberFunction(this, &PanelWidget::OnChildSizeDirty);

			Children.Add(&Widget);

			// Set the widget's parent
			Widget.SetParent(this);
		}

		virtual void Show() override
		{
			for (Widget* Child : Children)
			{
				Child->Show();
			}
		}

		virtual void Hide() override
		{
			for (Widget* Child : Children)
			{
				Child->Hide();
			}
		}

		virtual void Draw() override
		{
			for (Widget* Child : Children)
			{
				Child->Draw();
			}
		}

		PanelWidget& operator+(Ry::Widget& Widget)
		{
			AppendSlot(Widget);

			return *this;
		}

		void SetShapeBatch(Batch* Shape) override
		{
			Widget::SetShapeBatch(Shape);

			for(Widget* Sl : Children)
			{
				Sl->SetShapeBatch(Shape);
			}
		}

		void SetTextBatch(Batch* Text) override
		{
			Widget::SetTextBatch(Text);

			for (Widget* Sl : Children)
			{
				Sl->SetTextBatch(Text);
			}
		}

		void SetTextureBatch(Batch* Text) override
		{
			Widget::SetTextureBatch(Text);

			for (Widget* Sl : Children)
			{
				Sl->SetTextureBatch(Text);
			}
		}

	protected:

		Ry::ArrayList<Widget*> Children;
	};

}
