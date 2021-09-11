#pragma once

#include "Widget/Widget.h"
#include "RenderingEngine.h"
#include "RenderingPass.h"
#include "Font.h"
#include "Color.h"
#include "Style.h"
#include "UIGen.h"

namespace Ry
{
	class UI_MODULE Label : public Widget
	{
	public:

		WidgetBeginArgs(Label)
			WidgetProp(Ry::String, Text)
			WidgetProp(BitmapFont*, Font)
			WidgetProp(Ry::Color, Color)
		WidgetEndArgs()

		void Construct(Args& In)
		{
			this->SetText(In.mText);
			this->SetStyle(In.mFont, In.mColor);
		}

		Label():
		Widget()
		{
			ItemSet = MakeItemSet();
		}

		SizeType ComputeSize() const override
		{
			SizeType Result;
			Result.Width = static_cast<int32>(MaxSize.Width > 0 ? MaxSize.Width : Style.Font->MeasureWidth(Text));
			Result.Height = static_cast<int32>(Style.Font->MeasureHeight(Text, static_cast<float>(Result.Width)));
			return Result;
		}

		Label& SetText(const Ry::String& Text)
		{
			this->Text = Text;

			RenderStateDirty.Broadcast();
			
			return *this;
		}

		Label& SetStyle(BitmapFont* Font, const Color& Color)
		{
			Style.SetFont(Font).SetColor(Color);

			return *this;
		}

		Label& SetFont(BitmapFont* Font)
		{
			Style.SetFont(Font);
			
			RenderStateDirty.Broadcast();
			return *this;
		}

		void OnShow() override
		{
			if(Bat)
			{
				Bat->AddItemSet(ItemSet, "Font", GetClipSpace(), Style.Font->GetAtlasTexture(), WidgetLayer);
			}
		}

		void OnHide() override
		{
			if(Bat)
			{
				Bat->RemoveItemSet(ItemSet);
			}
		}

		void Draw() override
		{
			Point Abs = GetAbsolutePosition();
			Ry::BatchText(ItemSet, Style.TextColor, Style.Font, Text, static_cast<float>(Abs.X), static_cast<float>(Abs.Y), static_cast<float>(ComputeSize().Width));

			//TextBatch->SetTexture(Style.Font->GetAtlasTexture());
		}

	private:
		
		String Text;
		TextStyle Style;
		Ry::SharedPtr<BatchItemSet> ItemSet;
	};
}
