#pragma once

#include "Widget/Widget.h"
#include "Color.h"
#include "Drawable.h"
#include "Style.h"
#include "UIGen.h"
#include "SlotWidget.h"

namespace Ry
{
	class UI_MODULE BorderWidget : public SlotWidget
	{
	public:

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

		void OnShow() override
		{
			if(IsHovered())
			{
				if (IsPressed())
				{
					if(Style.Pressed)
					{
						Style.Pressed->Show();
					}
					else if(Style.Hovered)
					{
						Style.Hovered->Show();
					}
					else if(Style.Default)
					{
						Style.Default->Show();
					}
				}
				else
				{
					if(Style.Hovered)
					{
						Style.Hovered->Show();
					}
					else if(Style.Default)
					{
						Style.Default->Show();
					}
				}
			}
			else
			{
				if(Style.Default)
				{
					Style.Default->Show();
				}
			}
		}
		
		void OnHide() override
		{
			if(Style.Default)
			{
				Style.Default->Hide();
			}

			if(Style.Pressed)
			{
				Style.Pressed->Hide();
			}

			if(Style.Hovered)
			{
				Style.Hovered->Hide();
			}
		}

		void Draw() override
		{
			// Note: super function is called later on
			
			Point Abs = GetAbsolutePosition();
			SizeType ComputedSize = ComputeSize();

			if(IsHovered())
			{
				if(IsPressed() && Style.Pressed)
				{
					Style.Pressed->Draw((float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float) ComputedSize.Height);
				}
				else if(Style.Hovered)
				{
					Style.Hovered->Draw((float)Abs.X, (float)Abs.Y, (float)ComputedSize.Width, (float)ComputedSize.Height);
				}
			}
			else if(Style.Default)
			{
				Style.Default->Draw((float) Abs.X, (float) Abs.Y, (float) ComputedSize.Width, (float) ComputedSize.Height);
			}

			SlotWidget::Draw();
		}

		bool HasVisual()
		{
			return Style.Default || Style.Hovered || Style.Pressed;
		}

		void SetShapeBatch(Batch* Shape) override
		{
			SlotWidget::SetShapeBatch(Shape);

			if(Style.Default.IsValid())
			{
				Style.Default->ShapeBatch = Shape;
			}

			if(Style.Hovered.IsValid())
			{
				Style.Hovered->ShapeBatch = Shape;
			}

			if(Style.Pressed.IsValid())
			{
				Style.Pressed->ShapeBatch = Shape;
			}

		}

		void SetTextBatch(Batch* Text) override
		{
			SlotWidget::SetTextBatch(Text);

		}

		void SetTextureBatch(Batch* Text) override
		{
			SlotWidget::SetTextureBatch(Text);

			if (Style.Default.IsValid())
			{
				Style.Default->TextureBatch = Text;
			}

			if (Style.Hovered.IsValid())
			{
				Style.Hovered->TextureBatch = Text;
			}

			if (Style.Pressed.IsValid())
			{
				Style.Pressed->TextureBatch = Text;
			}
		}

		void OnHovered(const MouseEvent& MouseEv) override
		{
			SlotWidget::OnHovered(MouseEv);

			Refresh();
		}

		void OnUnhovered(const MouseEvent& MouseEv) override
		{
			SlotWidget::OnUnhovered(MouseEv);

			Refresh();

		}

		bool OnPressed(const MouseButtonEvent& MouseEv) override
		{
			SlotWidget::OnPressed(MouseEv);

			Refresh();

			return true;
		}

		bool OnReleased(const MouseButtonEvent& MouseEv) override
		{
			SlotWidget::OnReleased(MouseEv);

			Refresh();

			return true;
		}

	private:

		void Refresh()
		{
			// Ensures the correct border graphic is displayed
			OnHide();
			OnShow();

			// Construct border graphic mesh
			Draw();

			// Update the batches (todo: only shape and texture batches?)
			ShapeBatch->Update();
			TextBatch->Update();
			TextureBatch->Update();
		}

		/**
		 * The styling for various states of this box element.
		 */
		BoxStyle Style;
		
	};
}
