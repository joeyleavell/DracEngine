#include "Widget/BorderWidget.h"

namespace Ry
{

	BorderWidget::BorderWidget() : SlotWidget()
	{
	}

	void BorderWidget::Construct(Args& In)
	{
		SlotWidget::Args ParentArgs;
		ParentArgs.mPadding = In.mPadding;
		ParentArgs.mVerticalAlignment = In.mVertAlign;
		ParentArgs.mHorizontalAlignment = In.mHorAlign;
		ParentArgs.mFillX = In.mFillX;
		ParentArgs.mFillY = In.mFillY;
		ParentArgs.Children = In.Children;
		SlotWidget::Construct(ParentArgs);

		if (In.mDefaultImage.IsSet())
		{
			this->DefaultImage(In.mDefaultImage, In.mDefaultImageTint);
		}

		if (In.mDefaultBox.IsSet())
		{
			this->BStyle.DefaultBox().Set(In.mDefaultBox);
		}

		if (In.mHoveredImage.IsSet())
		{
			this->HoveredImage(In.mHoveredImage, In.mHoveredImageTint);
		}

		if (In.mHoveredBox.IsSet())
		{
			this->BStyle.HoveredBox().Set(In.mHoveredBox);
		}

		if (In.mPressedImage.IsSet())
		{
			this->PressedImage(In.mPressedImage, In.mPressedImageTint);
		}

		if (In.mPressedBox.IsSet())
		{
			this->BStyle.PressedBox().Set(In.mPressedBox);
		}

	}

	BorderWidget& BorderWidget::Box(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
	{
		return DefaultBox(BackgroundColor, BorderColor, BorderRadius, BorderSize).HoveredBox(BackgroundColor, BorderColor, BorderRadius, BorderSize).PressedBox(BackgroundColor, BorderColor, BorderRadius, BorderSize);
	}

	BorderWidget& BorderWidget::DefaultImage(Texture* Image, Color ImageTint)
	{
		BStyle.DefaultBoxImage().SetImageTint(ImageTint).SetTexture(Image);
		return *this;
	}

	BorderWidget& BorderWidget::HoveredImage(Texture* Image, Color ImageTint)
	{
		BStyle.HoveredBoxImage().SetImageTint(ImageTint).SetTexture(Image);
		return *this;
	}

	BorderWidget& BorderWidget::PressedImage(Texture* Image, Color ImageTint)
	{
		BStyle.PressedBoxImage().SetImageTint(ImageTint).SetTexture(Image);
		return *this;
	}

	BorderWidget& BorderWidget::DefaultBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
	{
		BStyle.DefaultBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
		return *this;
	}

	BorderWidget& BorderWidget::HoveredBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
	{
		BStyle.HoveredBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
		return *this;
	}

	BorderWidget& BorderWidget::PressedBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
	{
		BStyle.PressedBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
		return *this;
	}

	void BorderWidget::OnShow(Ry::Batch* Batch)
	{
		if (IsHovered())
		{
			if (IsPressed())
			{
				if (BStyle.Pressed)
				{
					BStyle.Pressed->Show(Batch, WidgetLayer, GetPipelineState(this));
				}
				else if (BStyle.Hovered)
				{
					BStyle.Hovered->Show(Batch, WidgetLayer, GetPipelineState(this));
				}
				else if (BStyle.Default)
				{
					BStyle.Default->Show(Batch, WidgetLayer, GetPipelineState(this));
				}
			}
			else
			{
				if (BStyle.Hovered)
				{
					BStyle.Hovered->Show(Batch, WidgetLayer, GetPipelineState(this));
				}
				else if (BStyle.Default)
				{
					BStyle.Default->Show(Batch, WidgetLayer, GetPipelineState(this));
				}
			}
		}
		else
		{
			if (BStyle.Default)
			{
				BStyle.Default->Show(Batch, WidgetLayer, GetPipelineState(this));
			}
		}
	}

	void BorderWidget::OnHide(Ry::Batch* Batch)
	{
		if (BStyle.Default)
		{
			BStyle.Default->Hide(Batch);
		}

		if (BStyle.Pressed)
		{
			BStyle.Pressed->Hide(Batch);
		}

		if (BStyle.Hovered)
		{
			BStyle.Hovered->Hide(Batch);
		}
	}

	void BorderWidget::Draw()
	{
		// Note: super function is called later on

		if (IsVisible())
		{
			Point Abs = GetAbsolutePosition();
			SizeType ComputedSize = ComputeSize();

			// choose from :default :hovered and :pressed styles
			if (IsHovered())
			{
				if (IsPressed() && BStyle.Pressed)
				{
					BStyle.Pressed->Draw((float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
				}
				else if (BStyle.Hovered)
				{
					BStyle.Hovered->Draw((float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
				}
			}
			else if (BStyle.Default)
			{
				BStyle.Default->Draw((float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
			}

		}

		SlotWidget::Draw();
	}

	bool BorderWidget::HasVisual()
	{
		return BStyle.Default || BStyle.Hovered || BStyle.Pressed;
	}

	void BorderWidget::OnHovered(const MouseEvent& MouseEv)
	{
		SlotWidget::OnHovered(MouseEv);

		MarkDirty(this);
	}

	void BorderWidget::OnUnhovered(const MouseEvent& MouseEv)
	{
		SlotWidget::OnUnhovered(MouseEv);

		MarkDirty(this);

	}

	bool BorderWidget::OnPressed(const MouseButtonEvent& MouseEv)
	{
		SlotWidget::OnPressed(MouseEv);

		MarkDirty(this);

		return true;
	}

	bool BorderWidget::OnReleased(const MouseButtonEvent& MouseEv)
	{
		SlotWidget::OnReleased(MouseEv);

		MarkDirty(this);

		return true;
	}

}