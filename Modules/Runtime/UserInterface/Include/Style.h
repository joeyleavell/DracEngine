#pragma once

#include "Color.h"
#include "Font.h"
#include "Drawable.h"

namespace Ry
{

	class USERINTERFACE_MODULE TextStyle
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

	class USERINTERFACE_MODULE BoxStyle
	{
	public:
		Ry::SharedPtr<Drawable> Default;
		Ry::SharedPtr<Drawable> Hovered;
		Ry::SharedPtr<Drawable> Pressed;

		ImageDrawable& DefaultBoxImage()
		{
			ImageDrawable* Existing = dynamic_cast<ImageDrawable*>(Default.Get());

			if (!Existing)
			{
				Default.Reset(Existing = new ImageDrawable);
			}

			return *Existing;
		}

		ImageDrawable& HoveredBoxImage()
		{
			ImageDrawable* Existing = dynamic_cast<ImageDrawable*>(Hovered.Get());

			if (!Existing)
			{
				Hovered.Reset(Existing = new ImageDrawable);
			}

			return *Existing;
		}
		
		ImageDrawable& PressedBoxImage()
		{
			ImageDrawable* Existing = dynamic_cast<ImageDrawable*>(Pressed.Get());

			if (!Existing)
			{
				Pressed.Reset(Existing = new ImageDrawable);
			}

			return *Existing;
		}

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
				Hovered.Reset(Existing = new BoxDrawable);
			}

			return *Existing;
		}

		BoxDrawable& PressedBox()
		{
			BoxDrawable* Existing = dynamic_cast<BoxDrawable*>(Pressed.Get());

			if (!Existing)
			{
				Pressed.Reset(Existing = new BoxDrawable);
			}

			return *Existing;
		}
	};

	class USERINTERFACE_MODULE StyleSet
	{
	public:

		StyleSet();
		virtual ~StyleSet();

		void AddTextStyle(Ry::String Name, const TextStyle& TextStyle);
		void AddBoxStyle(Ry::String Name, const BoxStyle& BoxStyle);

		void RemoveTextStyle(Ry::String Name);
		void RemoveBoxStyle(Ry::String Name);

		const TextStyle& GetTextStyle(Ry::String Name) const;
		const BoxStyle& GetBoxStyle(Ry::String Name) const;

	private:

		Ry::Map<Ry::String, TextStyle> TextStyles;
		Ry::Map<Ry::String, BoxStyle> BoxStyles;

	};

	extern USERINTERFACE_MODULE Map<Ry::String, const StyleSet*> RegisteredStyles;

	USERINTERFACE_MODULE void RegisterStyle(Ry::String Name, const StyleSet* Set);
	USERINTERFACE_MODULE void UnregisterStyle(Ry::String Name);

	USERINTERFACE_MODULE const StyleSet* GetStyle(Ry::String Name);

	extern USERINTERFACE_MODULE StyleSet EditorStyle;
	
}
