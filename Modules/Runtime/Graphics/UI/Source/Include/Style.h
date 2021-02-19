#pragma once

#include "Core/Core.h"
#include "Color.h"
#include "Font.h"
#include "Drawable.h"

namespace Ry
{

	class UI_MODULE TextStyle
	{
	public:
		
		BitmapFont* Font;
		Color TextColor;

		TextStyle()
		{
			this->Font = nullptr;
			this->TextColor = Ry::WHITE;
		}

		TextStyle& SetFont(BitmapFont* Font)
		{
			this->Font = Font;

			return *this;
		}

		TextStyle& SetColor(const Color& Color)
		{
			this->TextColor = Color;

			return *this;
		}

	};

	class UI_MODULE BoxStyle
	{
	public:
		Ry::SharedPtr<Drawable> Default;
		Ry::SharedPtr<Drawable> Hovered;
		Ry::SharedPtr<Drawable> Pressed;

		BoxDrawable& DefaultBox()
		{
			BoxDrawable* Existing = dynamic_cast<BoxDrawable*>(Default.Get());

			if (!Existing)
			{
				Default.Reset(Existing = new BoxDrawable);
			}

			return *Existing;
		}

		BoxDrawable& HoveredBox()
		{
			BoxDrawable* Existing = dynamic_cast<BoxDrawable*>(Hovered.Get());

			if (!Existing)
			{
				Default.Reset(Existing = new BoxDrawable);
			}

			return *Existing;
		}

		BoxDrawable& PressedBox()
		{
			BoxDrawable* Existing = dynamic_cast<BoxDrawable*>(Pressed.Get());

			if (!Existing)
			{
				Default.Reset(Existing = new BoxDrawable);
			}

			return *Existing;
		}
	};

	
}
