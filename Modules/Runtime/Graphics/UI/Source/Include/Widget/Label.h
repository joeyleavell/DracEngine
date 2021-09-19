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

			this->bTextSizeDirty = true;
		}

		Label():
		Widget()
		{
			ItemSet = MakeItemSet();
		}

		SizeType ComputeSize() const override
		{
			if(bTextSizeDirty)
			{
				CachedSize.Width = static_cast<int32>(MaxSize.Width > 0 ? MaxSize.Width : Style.Font->MeasureWidth(Text));
				CachedSize.Height = static_cast<int32>(Style.Font->MeasureHeight(Text, static_cast<float>(CachedSize.Width)));
				bTextSizeDirty = false;
			}

			return CachedSize;
		}

		Label& SetText(const Ry::String& Text)
		{
			this->Text = Text;
			bTextSizeDirty = true;

			// Pre compute text data
			ComputeTextData();

			RenderStateDirty.Broadcast();
			
			return *this;
		}

		Label& SetStyle(BitmapFont* Font, const Color& Color)
		{
			Style.SetFont(Font).SetColor(Color);
			bTextSizeDirty = true;

			return *this;
		}

		Label& SetFont(BitmapFont* Font)
		{
			Style.SetFont(Font);
			bTextSizeDirty = true;
			
			RenderStateDirty.Broadcast();
			return *this;
		}

		const Ry::String& GetText() const
		{
			return Text;
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
			if(IsVisible())
			{
				Point Abs = GetAbsolutePosition();
				Ry::BatchText(ItemSet, Style.TextColor, Style.Font, ComputedTextData, static_cast<float>(Abs.X), static_cast<float>(Abs.Y), static_cast<float>(ComputeSize().Width));
			}

			//TextBatch->SetTexture(Style.Font->GetAtlasTexture());
		}

	private:

		void ComputeTextData()
		{
			ComputedTextData.Lines.Clear();
			
			Ry::StringView* Lines = nullptr;
			int32 LineCount = Text.split("\n", &Lines);
			for (int32 Line = 0; Line < LineCount; Line++)
			{
				TextLine NewLine;

				Ry::StringView* Words = nullptr;
				int32 WordCount = Lines[Line].split(" ", &Words);
				for (int32 Word = 0; Word < WordCount; Word++)
				{
					NewLine.Words.Add(Words[Word]);
				}

				// Add the cached line
				ComputedTextData.Lines.Add(NewLine);

				delete[] Words;
			}
			delete[] Lines;
		}

		mutable SizeType CachedSize;
		mutable bool bTextSizeDirty;
		PrecomputedTextData ComputedTextData;
		
		String Text;
		
		TextStyle Style;
		Ry::SharedPtr<BatchItemSet> ItemSet;
	};
}
