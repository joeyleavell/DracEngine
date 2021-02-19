#pragma once

#include "Widget/Widget.h"
#include "Color.h"
#include "Drawable.h"
#include "Style.h"
#include "UIGen.h"

namespace Ry
{
	class UI_MODULE BoxWidget : public Widget
	{
	public:

		BoxWidget():
		Widget()
		{
			// No child initially
			this->Child = nullptr;

			this->PaddingLeft = PaddingRight = PaddingTop = PaddingBottom = 0;
			this->Pressed = false;
			this->Hovered = false;

			this->WidthMode = SizeMode::AUTO;
			this->HeightMode = SizeMode::AUTO;

			this->VerticalAlign = VAlign::TOP;
			this->HorizontalAlign = HAlign::LEFT;
		}

		BoxWidget& Box(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
		{
			return DefaultBox(BackgroundColor, BorderColor, BorderRadius, BorderSize).HoveredBox(BackgroundColor, BorderColor, BorderRadius, BorderSize).PressedBox(BackgroundColor, BorderColor, BorderRadius, BorderSize);
		}

		BoxWidget& DefaultBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
		{
			Style.DefaultBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
			return *this;
		}

		BoxWidget& HoveredBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
		{
			Style.PressedBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
			return *this;
		}

		BoxWidget& PressedBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
		{
			Style.HoveredBox().SetBackgroundColor(BackgroundColor).SetBorderColor(BorderColor).SetBorderRadius(BorderRadius).SetBorderSize(BorderSize);
			return *this;
		}

		BoxWidget& Padding(float Pad)
		{
			return Padding(Pad, Pad);
		}

		BoxWidget& Padding(float Vertical, float Horizontal)
		{
			return Padding(Horizontal, Horizontal, Vertical, Vertical);
		}

		BoxWidget& Padding(float Left, float Right, float Top, float Bottom)
		{
			this->PaddingLeft = Left;
			this->PaddingRight = Right;
			this->PaddingTop = Top;
			this->PaddingBottom = Bottom;
			
			return *this;
		}


		BoxWidget& SetVerticalAlignment(VAlign Alignment)
		{
			this->ContentVAlign = Alignment;

			return *this;
		}

		BoxWidget& SetHorizontalAlignment(HAlign Alignment)
		{
			this->ContentHAlign = Alignment;

			return *this;
		}

		BoxWidget& AutoWidth()
		{
			this->WidthMode = SizeMode::AUTO;

			return *this;
		}

		BoxWidget& AutoHeight()
		{
			this->WidthMode = SizeMode::AUTO;

			return *this;
		}

		BoxWidget& FillX(float FillX)
		{
			this->FillXPercent = FillX;
			this->WidthMode = SizeMode::PERCENTAGE;

			return *this;
		}

		BoxWidget& FillY(float FillY)
		{
			this->FillYPercent = FillY;
			this->HeightMode = SizeMode::PERCENTAGE;

			return *this;
		}

		BoxWidget& FillParent()
		{
			return FillX(1.0).FillY(1.0);
		}

		BoxWidget& SetVAlign(VAlign VerticalAlign)
		{
			this->VerticalAlign = VerticalAlign;

			return *this;
		}

		BoxWidget& SetHAlign(HAlign HorizontalAlign)
		{
			this->HorizontalAlign = HorizontalAlign;

			return *this;
		}

		bool IsHovered()
		{
			return Hovered;
		}

		bool IsPressed()
		{
			return Pressed;
		}

		SizeType ComputeSize() const override
		{
			SizeType ParentSize;
			SizeType ChildSize;
			SizeType Result;

			// Compute child size
			if(Child)
			{
				ChildSize = Child->ComputeSize();
			}

			// Compute parent size
			if(Parent && (WidthMode == SizeMode::PERCENTAGE || HeightMode == SizeMode::PERCENTAGE))
			{
				ParentSize = Parent->ComputeSize();
			}
			else
			{
				ParentSize.Width = Ry::GetViewportWidth();
				ParentSize.Height = Ry::GetViewportHeight();
			}

			if(WidthMode == SizeMode::PERCENTAGE)
			{
				Result.Width = (int32) (ParentSize.Width * FillXPercent);
			}
			else if(WidthMode == SizeMode::AUTO)
			{
				Result.Width = (int32) (PaddingLeft + PaddingRight + ChildSize.Width);
			}

			if (HeightMode == SizeMode::PERCENTAGE)
			{
				Result.Height = (int32) (ParentSize.Height * FillYPercent);
			}
			else if (HeightMode == SizeMode::AUTO)
			{
				Result.Height = (int32) (PaddingTop + PaddingBottom + ChildSize.Height);
			}

			return Result;
		}

		void Show() override
		{
			if(Hovered)
			{
				if (Pressed)
				{
					if(Style.Pressed)
					{
						Style.Pressed->Show();
					}
				}
				else
				{
					if(Style.Hovered)
					{
						Style.Hovered->Show();
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

			if(Child)
			{
				Child->Show();
			}
		}
		
		void Hide() override
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
			Point Abs = GetAbsolutePosition();
			SizeType ComputedSize = ComputeSize();

			if(Hovered)
			{
				if(Pressed && Style.Pressed)
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

			// Only show default
			// Style.Default->Show();

			// Render child
			if (Child)
			{
				Child->Draw();
			}
		}

		void Arrange() override
		{
			if(Child)
			{
				SizeType ChildSize = Child->ComputeSize();
				float RelX = PaddingLeft;
				float RelY = PaddingTop;

				if(WidthMode == SizeMode::PERCENTAGE)
				{
					SizeType ThisSize = ComputeSize();
					switch((uint32) HorizontalAlign)
					{
					case (uint32) HAlign::CENTER:
						RelX = (ThisSize.Width - ChildSize.Width) / 2.0f;
						break;
					case (uint32) HAlign::LEFT:
						RelX = PaddingLeft;
						break;
					case (uint32) HAlign::RIGHT:
						RelX = (float) (ThisSize.Width - ChildSize.Width - PaddingRight);
						break;
					}
				}

				if (HeightMode == SizeMode::PERCENTAGE)
				{
					SizeType ThisSize = ComputeSize();
					switch ((uint32) VerticalAlign)
					{
					case (uint32)VAlign::CENTER:
						RelY = (ThisSize.Height - ChildSize.Height) / 2.0f;
						break;
					case (uint32)VAlign::TOP:
						RelY = PaddingTop;
						break;
					case (uint32)VAlign::BOTTOM:
						RelY = ThisSize.Height - ChildSize.Height - PaddingBottom;
						break;
					}
				}

				Child->SetRelativePosition(RelX, RelY);
				Child->Arrange();
			}
		}
		
		void SetChild(Ry::Widget& Child)
		{
			// TODO: if there is an existing child, remove the parent/child links

			// Setup the parent/child relationship
			this->Child = &Child;
			Child.SetParent(this);
			Child.SizeDirty.AddMemberFunction(this, &BoxWidget::Arrange);

			// Automatically rearrange
			Arrange();

			// Recompute cached size
			SizeDirty.Broadcast();
		}

		Widget& operator[](Ry::Widget& Child) override
		{
			SetChild(Child);

			return *this;
		}

	private:

		VAlign ContentVAlign;
		HAlign ContentHAlign;
		SizeMode WidthMode;
		SizeMode HeightMode;

		// Percentage parameters
		float FillXPercent;
		float FillYPercent;

		/**
		 * The child stored within this box element.
		 */
		Ry::Widget* Child;

		/**
		 * The padding of the inner content of this box element.
		 */
		float PaddingLeft, PaddingRight, PaddingTop, PaddingBottom;

		/**
		 * The styling for various states of this box element.
		 */
		BoxStyle Style;

		/**
		 * Whether this box element is currently hovered over.
		 */
		bool Hovered;

		/**
		 * Whether this box element is currently pressed.
		 */
		bool Pressed;

		VAlign VerticalAlign;
		HAlign HorizontalAlign;
	};
}
