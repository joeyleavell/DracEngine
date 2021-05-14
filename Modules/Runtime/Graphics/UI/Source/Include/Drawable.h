#pragma once

#include "Core/Core.h"
#include "RenderingEngine.h"
#include "2D/Batch/Batch.h"
#include "UIGen.h"

namespace Ry
{

	class UI_MODULE Drawable
	{
	public:

		Batch* ShapeBatch   = nullptr;
		Batch* TextureBatch = nullptr;

		Drawable() = default;
		virtual ~Drawable() = default;

		virtual void Show() = 0;
		virtual void Hide() = 0;
		virtual void Draw(float X, float Y, float Width, float Height) = 0;
	};

	class UI_MODULE ImageDrawable : public Drawable
	{
	public:
		ImageDrawable()
		{
			this->Item = Ry::MakeItem();
			this->ImageTint = Ry::WHITE;
			this->ParentTexture = nullptr;
		}

		ImageDrawable& SetTexture(Texture* Text)
		{
			this->ParentTexture = Text;

			return *this;
		}

		ImageDrawable& SetImageTint(const Color& Tint)
		{
			this->ImageTint = Tint;

			return *this;
		}

		void Show() override
		{
			TextureBatch->AddItem(Item, ParentTexture);
		}

		void Hide() override
		{
			TextureBatch->RemoveItem(Item);
		}

		// todo: make use this texture regions
		void Draw(float X, float Y, float Width, float Height) override
		{
			Ry::BatchTexture(Item, ImageTint,
				X, Y,
				0.0f, 0.0f,
				1.0f, 1.0f,
				0.5f, 0.5f,
				Width, Height,
				0.0f
			);
			
		}

	private:

		Color ImageTint;
		Ry::SharedPtr<BatchItem> Item;
		Texture* ParentTexture;

	};

	class UI_MODULE BoxDrawable : public Drawable
	{
	public:
		BoxDrawable()
		{
			this->BorderRadius = 0;
			this->BorderSize = 0;
			this->BackgroundColor = Ry::WHITE.ScaleRGB(0.4f);

			this->ItemSet = Ry::MakeItemSet();
		}

		BoxDrawable& SetBackgroundColor(const Color& BgColor)
		{
			this->BackgroundColor = BgColor;

			return *this;
		}

		BoxDrawable& SetBorderRadius(int32 BorderRad)
		{
			this->BorderRadius = BorderRad;

			return *this;
		}

		BoxDrawable& SetBorderSize(int32 BorderSize)
		{
			this->BorderSize = BorderSize;

			return *this;
		}
		
		BoxDrawable& SetBorderColor(const Color& BorderColor)
		{
			this->BorderColor = BorderColor;

			return *this;
		}

		void Show() override
		{
			ShapeBatch->AddItemSet(ItemSet);
		}

		void Hide() override
		{
			ShapeBatch->RemoveItemSet(ItemSet);
		}

		void Draw(float X, float Y, float Width, float Height) override
		{
			Ry::BatchStyledBox(ItemSet, X, Y, Width, Height, BackgroundColor, BorderColor, BorderRadius, BorderSize, 0.0f);

			// ShapeBatcher->begin(Ry::DrawMode::FILLED);
			// ShapeBatcher->DrawStyledBox(X, Y, Width, Height, BackgroundColor, BorderColor, BorderRadius, BorderSize);
			// ShapeBatcher->end();
		}
		
	private:
		Color BackgroundColor;
		int32 BorderRadius;
		int32 BorderSize;
		Color BorderColor;
		Ry::SharedPtr<BatchItemSet> ItemSet;
		
	};
	
}
