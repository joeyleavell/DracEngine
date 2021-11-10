#pragma once

#include "Widget/Widget.h"
#include "Color.h"
#include "Drawable.h"
#include "Style.h"
#include "SlotWidget.h"
#include "UserInterfaceGen.h"

namespace Ry
{
	class USERINTERFACE_MODULE BorderWidget : public SlotWidget
	{
	public:

		// .DefaultImage(UpArrowTexture, WHITE)
		// 	.HoveredImage(UpArrowTexture, WHITE.ScaleRGB(0.8f))
		// 	.PressedImage(UpArrowTexture, WHITE.ScaleRGB(0.6f))
		// 	.Padding(10.0f)

		WidgetBeginArgs(BorderWidget)
			WidgetProp(float, FillX)
			WidgetProp(float, FillY)
			WidgetPropDefault(float, Padding, 0.0f)
			WidgetPropDefault(HAlign, HorAlign, HAlign::CENTER)
			WidgetPropDefault(VAlign, VertAlign, VAlign::CENTER)
			WidgetPropDefault(bool, HasStyle, true)
			WidgetProp(Texture*, DefaultImage)
			WidgetProp(Color, DefaultImageTint)
			WidgetProp(Texture*, HoveredImage)
			WidgetProp(Color, HoveredImageTint)
			WidgetProp(Texture*, PressedImage)
			WidgetProp(Color, PressedImageTint)
			WidgetProp(BoxDrawable, DefaultBox)
			WidgetProp(BoxDrawable, HoveredBox)
			WidgetProp(BoxDrawable, PressedBox)
		WidgetEndArgs()
		
		void Construct(Args& In)
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
		
			if(In.mDefaultBox.IsSet())
			{
				this->Style.DefaultBox().Set(In.mDefaultBox);
			}

			if (In.mHoveredImage.IsSet())
			{
				this->HoveredImage(In.mHoveredImage, In.mHoveredImageTint);
			}

			if(In.mHoveredBox.IsSet())
			{
				this->Style.HoveredBox().Set(In.mHoveredBox);
			}

			if (In.mPressedImage.IsSet())
			{
				this->PressedImage(In.mPressedImage, In.mPressedImageTint);
			}
		
			if(In.mPressedBox.IsSet())
			{
				this->Style.PressedBox().Set(In.mPressedBox);
			}

		}

		BorderWidget():
		SlotWidget()
		{
		}

		BorderWidget& Box(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
		{
			return DefaultBox(BackgroundColor, BorderColor, BorderRadius, BorderSize).HoveredBox(BackgroundColor, BorderColor, BorderRadius, BorderSize).PressedBox(BackgroundColor, BorderColor, BorderRadius, BorderSize);
		}

		BorderWidget& DefaultImage(Texture* Image, Color ImageTint = WHITE)
		{
			Style.DefaultBoxImage().SetImageTint(ImageTint).SetTexture(Image);
			return *this;
		}

		BorderWidget& HoveredImage(Texture* Image, Color ImageTint = WHITE)
		{
			Style.HoveredBoxImage().SetImageTint(ImageTint).SetTexture(Image);
			return *this;
		}
		
		BorderWidget& PressedImage(Texture* Image, Color ImageTint = WHITE)
		{
			Style.PressedBoxImage().SetImageTint(ImageTint).SetTexture(Image);
			return *this;
		}
		
		BorderWidget& DefaultBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
		{
			Style.DefaultBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
			return *this;
		}

		BorderWidget& HoveredBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
		{
			Style.HoveredBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
			return *this;
		}

		BorderWidget& PressedBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
		{
			Style.PressedBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
			return *this;
		}

		void OnShow(Ry::Batch* Batch) override
		{
			if(IsHovered())
			{
				if (IsPressed())
				{
					if(Style.Pressed)
					{
						Style.Pressed->Show(Batch, WidgetLayer, GetPipelineState(this));
					}
					else if(Style.Hovered)
					{
						Style.Hovered->Show(Batch, WidgetLayer, GetPipelineState(this));
					}
					else if(Style.Default)
					{
						Style.Default->Show(Batch, WidgetLayer, GetPipelineState(this));
					}
				}
				else
				{
					if(Style.Hovered)
					{
						Style.Hovered->Show(Batch, WidgetLayer, GetPipelineState(this));
					}
					else if(Style.Default)
					{
						Style.Default->Show(Batch, WidgetLayer, GetPipelineState(this));
					}
				}
			}
			else
			{
				if(Style.Default)
				{
					Style.Default->Show(Batch, WidgetLayer, GetPipelineState(this));
				}
			}
		}
		
		void OnHide(Ry::Batch* Batch) override
		{
			if(Style.Default)
			{
				Style.Default->Hide(Batch);
			}

			if(Style.Pressed)
			{
				Style.Pressed->Hide(Batch);
			}

			if(Style.Hovered)
			{
				Style.Hovered->Hide(Batch);
			}
		}

		void Draw(StyleSet* TheStyle) override
		{
			// Note: super function is called later on

			if(IsVisible())
			{
				Point Abs = GetAbsolutePosition();
				SizeType ComputedSize = ComputeSize();

				// choose from :default :hovered and :pressed styles
				if (IsHovered())
				{
					if (IsPressed() && Style.Pressed)
					{
						Style.Pressed->Draw((float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
					}
					else if (Style.Hovered)
					{
						Style.Hovered->Draw((float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
					}
				}
				else if (Style.Default)
				{
					Style.Default->Draw((float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
				}

			}

			SlotWidget::Draw(TheStyle);
		}

		bool HasVisual()
		{
			return Style.Default || Style.Hovered || Style.Pressed;
		}

		// void SetBatch(Ry::Batch* Bat) override
		// {
		// 	SlotWidget::SetBatch(Bat);
		//
		// 	if(Style.Default.IsValid())
		// 	{
		// 		Style.Default->Bat = Bat;
		// 	}
		//
		// 	if(Style.Hovered.IsValid())
		// 	{
		// 		Style.Hovered->Bat = Bat;
		// 	}
		//
		// 	if(Style.Pressed.IsValid())
		// 	{
		// 		Style.Pressed->Bat = Bat;
		// 	}
		//
		// }

		void OnHovered(const MouseEvent& MouseEv) override
		{
			SlotWidget::OnHovered(MouseEv);

			MarkDirty(this);
		}

		void OnUnhovered(const MouseEvent& MouseEv) override
		{
			SlotWidget::OnUnhovered(MouseEv);

			MarkDirty(this);

		}

		bool OnPressed(const MouseButtonEvent& MouseEv) override
		{
			SlotWidget::OnPressed(MouseEv);

			MarkDirty(this);

			return true;
		}

		bool OnReleased(const MouseButtonEvent& MouseEv) override
		{
			SlotWidget::OnReleased(MouseEv);

			MarkDirty(this);
			
			return true;
		}

	private:

	/*	void Refresh()
		{
			// Ensures the correct border graphic is displayed
			if(IsVisible())
			{
				OnHide();
				OnShow();
			}

			// todo: fix these

			// Construct border graphic mesh
			Draw(nullptr);

			// Update the batch
			Bat->Update();
		}*/

		/**
		 * The styling for various states of this box element.
		 */
		BoxStyle Style;
		
	};
}
