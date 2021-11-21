#include "Widget/Label.h"

namespace Ry
{

	Label::Label() :
		Widget()
	{
		ItemSet = MakeItemSet();
	}

	void Label::Construct(Args& In)
	{
		this->SetText(In.mText);
		this->SetStyle(In.mFont, In.mColor);

		this->bTextSizeDirty = true;
	}


	SizeType Label::ComputeSize() const
	{
		if (bTextSizeDirty)
		{
			CachedSize.Width = static_cast<int32>(MaxSize.Width > 0 ? MaxSize.Width : Style.Font->MeasureWidth(Text));
			CachedSize.Height = static_cast<int32>(Style.Font->MeasureHeight(Text, static_cast<float>(CachedSize.Width)));
			bTextSizeDirty = false;
		}

		return CachedSize;
	}

	Label& Label::SetText(const Ry::String& Text)
	{
		this->Text = Text;
		bTextSizeDirty = true;

		// Pre compute text data
		ComputeTextData();

		MarkDirty(this);

		return *this;
	}

	Label& Label::SetStyle(BitmapFont* Font, const Color& Color)
	{
		Style.SetFont(Font).SetColor(Color);
		bTextSizeDirty = true;

		return *this;
	}

	Label& Label::SetFont(BitmapFont* Font)
	{
		Style.SetFont(Font);
		bTextSizeDirty = true;

		MarkDirty(this);
		return *this;
	}

	const Ry::String& Label::GetText() const
	{
		return Text;
	}

	void Label::OnShow(Ry::Batch* Batch)
	{
		Batch->AddItemSet(ItemSet, "Font", GetPipelineState(this), Style.Font->GetAtlasTexture(), WidgetLayer);
	}

	void Label::OnHide(Ry::Batch* Batch)
	{
		Batch->RemoveItemSet(ItemSet);
	}

	void Label::Draw(StyleSet* TheStyle)
	{
		if (IsVisible())
		{
			Point Abs = GetAbsolutePosition();
			SizeType TextSize = ComputeSize();
			Ry::BatchText(ItemSet, Style.TextColor, Style.Font, ComputedTextData, static_cast<float>(Abs.X), static_cast<float>(Abs.Y + TextSize.Height), static_cast<float>(ComputeSize().Width));
		}
	}

	void Label::ComputeTextData()
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

}