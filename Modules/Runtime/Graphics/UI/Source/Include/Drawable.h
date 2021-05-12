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

		Batch* ShapeBatch = nullptr;

		Drawable() = default;
		virtual ~Drawable() = default;

		virtual void Show() = 0;
		virtual void Hide() = 0;
		virtual void Draw(float X, float Y, float Width, float Height) = 0;
	};

	class UI_MODULE BoxDrawable : public Drawable
	{
	public:
		BoxDrawable()
		{
			this->BackgroundColor = Ry::WHITE.ScaleRGB(0.8f);
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
