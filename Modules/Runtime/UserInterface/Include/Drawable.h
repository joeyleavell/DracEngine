#pragma once

#include "RenderingEngine.h"
#include "2D/Batch/Batch.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE Drawable
	{
	public:

		Drawable() = default;
		virtual ~Drawable() = default;

		virtual void Show(Ry::SharedPtr<BatchItemSet> ItemSet, Ry::Batch* Batch, int32 Layer, PipelineState State) = 0;
		virtual void Hide(Ry::SharedPtr<BatchItemSet> ItemSet, Ry::Batch* Batch) = 0;
		virtual void Draw(Ry::SharedPtr<BatchItemSet> ItemSet, float X, float Y, float Width, float Height) = 0;
	};

	class USERINTERFACE_MODULE ImageDrawable : public Drawable
	{
	public:
		ImageDrawable()
		{
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

		void Show(Ry::SharedPtr<BatchItemSet> ItemSet, Ry::Batch* Batch, int32 Layer, PipelineState State) override
		{
			Batch->AddItemSet(ItemSet, "Texture", State, ParentTexture, Layer);
		}

		void Hide(Ry::SharedPtr<BatchItemSet> ItemSet, Ry::Batch* Batch) override
		{
			Batch->RemoveItemSet(ItemSet);
		}

		void Draw(Ry::SharedPtr<BatchItemSet> ItemSet, float X, float Y, float Width, float Height) override
		{
			ItemSet->Items.Clear();

			Ry::SharedPtr<BatchItem> Item = MakeItem();
			ItemSet->AddItem(Item);

			Ry::BatchTexture(Item, ImageTint,
				X, Y,
				0.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 0.0f,
				Width, Height,
				0.0f
			);	
		}

	private:

		Color ImageTint;
		Texture* ParentTexture;

	};

	class USERINTERFACE_MODULE BoxDrawable : public Drawable
	{
	public:
		
		BoxDrawable()
		{
			this->BorderRadius = 0;
			this->BorderSize = 0;
			this->BackgroundColor = Ry::WHITE.ScaleRGB(0.4f);
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

		void Set(const BoxDrawable& Other)
		{
			this->BackgroundColor = Other.BackgroundColor;
			this->BorderColor = Other.BorderColor;
			this->BorderRadius = Other.BorderRadius;
			this->BorderSize = Other.BorderSize;
		}

		void Show(Ry::SharedPtr<BatchItemSet> ItemSet, Ry::Batch* Batch, int32 Layer, PipelineState State) override
		{
			Batch->AddItemSet(ItemSet, "Shape", State, nullptr, Layer);
		}

		void Hide(Ry::SharedPtr<BatchItemSet> ItemSet, Ry::Batch* Batch) override
		{
			Batch->RemoveItemSet(ItemSet);
		}

		void Draw(Ry::SharedPtr<BatchItemSet> ItemSet, float X, float Y, float Width, float Height) override
		{
			Ry::BatchStyledBox(ItemSet, X, Y, Width, Height, BackgroundColor, BorderColor, BorderRadius, BorderSize, 0.0f);
		}
		
	private:
		
		Color BackgroundColor;
		int32 BorderRadius;
		int32 BorderSize;
		Color BorderColor;
		
	};
	
}
