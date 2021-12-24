#include "Widget/BorderWidget.h"

namespace Ry
{

	BorderWidget::BorderWidget() : SlotWidget()
	{
		ItemSet = MakeItemSet();
	}

	void BorderWidget::OnShow(Ry::Batch* Batch)
	{
		if(!BoxStyleName.IsEmpty())
		{
			const BoxStyle& BoxStyle = Style->GetBoxStyle(BoxStyleName);

			if (IsHovered())
			{
				if (IsPressed())
				{

					if (BoxStyle.Pressed)
					{
						BoxStyle.Pressed->Show(ItemSet, Batch, WidgetLayer, GetPipelineState(this));
					}
					else if (BoxStyle.Hovered)
					{
						BoxStyle.Hovered->Show(ItemSet, Batch, WidgetLayer, GetPipelineState(this));
					}
					else if (BoxStyle.Default)
					{
						BoxStyle.Default->Show(ItemSet, Batch, WidgetLayer, GetPipelineState(this));
					}
				}
				else
				{
					if (BoxStyle.Hovered)
					{
						BoxStyle.Hovered->Show(ItemSet, Batch, WidgetLayer, GetPipelineState(this));
					}
					else if (BoxStyle.Default)
					{
						BoxStyle.Default->Show(ItemSet, Batch, WidgetLayer, GetPipelineState(this));
					}
				}
			}
			else
			{
				if (BoxStyle.Default)
				{
					BoxStyle.Default->Show(ItemSet, Batch, WidgetLayer, GetPipelineState(this));
				}
			}

		}
	}

	void BorderWidget::OnHide(Ry::Batch* Batch)
	{
		if(!BoxStyleName.IsEmpty())
		{
			const BoxStyle& BoxStyle = Style->GetBoxStyle(BoxStyleName);

			if (BoxStyle.Default)
			{
				BoxStyle.Default->Hide(ItemSet, Batch);
			}

			if (BoxStyle.Pressed)
			{
				BoxStyle.Pressed->Hide(ItemSet, Batch);
			}

			if (BoxStyle.Hovered)
			{
				BoxStyle.Hovered->Hide(ItemSet, Batch);
			}

		}
	}

	void BorderWidget::Draw()
	{
		
		if(!BoxStyleName.IsEmpty())
		{
			const BoxStyle& BoxStyle = Style->GetBoxStyle(BoxStyleName);

			// Note: super function is called later on

			if (IsVisible())
			{
				Point Abs = GetAbsolutePosition();
				SizeType ComputedSize = ComputeSize();

				// choose from :default :hovered and :pressed styles
				if (IsHovered())
				{
					if (IsPressed() && BoxStyle.Pressed)
					{
						BoxStyle.Pressed->Draw(ItemSet, (float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
					}
					else if (BoxStyle.Hovered)
					{
						BoxStyle.Hovered->Draw(ItemSet, (float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
					}
				}
				else if (BoxStyle.Default)
				{
					BoxStyle.Default->Draw(ItemSet, (float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
				}

			}
		}

		SlotWidget::Draw();
	}

	bool BorderWidget::HasVisual()
	{
		const BoxStyle& BoxStyle = Style->GetBoxStyle(BoxStyleName);

		return BoxStyle.Default || BoxStyle.Hovered || BoxStyle.Pressed;
	}

	void BorderWidget::OnHovered(const MouseEvent& MouseEv)
	{
		SlotWidget::OnHovered(MouseEv);

		RearrangeAndRepaint();
	}

	void BorderWidget::OnUnhovered(const MouseEvent& MouseEv)
	{
		SlotWidget::OnUnhovered(MouseEv);

		RearrangeAndRepaint();
	}

	bool BorderWidget::OnPressed(const MouseButtonEvent& MouseEv)
	{
		SlotWidget::OnPressed(MouseEv);

		RearrangeAndRepaint();

		return true;
	}

	bool BorderWidget::OnReleased(const MouseButtonEvent& MouseEv)
	{
		SlotWidget::OnReleased(MouseEv);

		RearrangeAndRepaint();

		return true;
	}

}