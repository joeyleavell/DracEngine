#include "Widget/Label.h"

namespace Ry
{

	Label::Label() :
		Widget()
	{
		ItemSet = MakeItemSet();
		MinimumWidth = 200.0f;
	}

	SizeType Label::ComputeSize() const
	{
		if (bTextSizeDirty && Style)
		{
			const TextStyle& ResolvedTextStyle = Style->GetTextStyle(TextStyleName);

			CachedSize.Width = static_cast<int32>(ResolvedTextStyle.Font->MeasureWidth(Text));
			CachedSize.Height = static_cast<int32>(ResolvedTextStyle.Font->MeasureHeight(Text, static_cast<float>(CachedSize.Width)));

			if(MinimumWidth >= 0 && CachedSize.Width < MinimumWidth)
			{
				CachedSize.Width = MinimumWidth;
			}

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

		FullRefresh();

		return *this;
	}

	Label& Label::SetTextStyle(const Ry::String& StyleName)
	{
		this->TextStyleName = StyleName;
		
		bTextSizeDirty = true;

		FullRefresh();

		return *this;
	}

	const Ry::String& Label::GetText() const
	{
		return Text;
	}

	void Label::OnShow(Ry::Batch* Batch)
	{
		// Check if text data needs to be computed. This handles the case where the text was loaded through reflection.
		if (ComputedTextData.Lines.GetSize() == 0 && !Text.IsEmpty())
		{
			SetText(Text);
		}

		const TextStyle& TextStyle = Style->GetTextStyle(TextStyleName);

		Batch->AddItemSet(ItemSet, "Font", GetPipelineState(this), TextStyle.Font->GetAtlasTexture(), WidgetLayer);
	}

	void Label::OnHide(Ry::Batch* Batch)
	{
		Batch->RemoveItemSet(ItemSet);
	}

	void Label::Draw()
	{

		if (IsVisible())
		{
			const TextStyle& TextStyle = Style->GetTextStyle(TextStyleName);

			Point Abs = GetAbsolutePosition();
			SizeType TextSize = ComputeSize();
			Ry::BatchText(ItemSet, TextStyle.TextColor, TextStyle.Font, ComputedTextData, static_cast<float>(Abs.X), static_cast<float>(Abs.Y + TextSize.Height), static_cast<float>(ComputeSize().Width));
		}
	}

	void Label::SetVisibleFlag(bool bVisibility, bool bPropagate)
	{
		Widget::SetVisibleFlag(bVisibility, bPropagate);

		ItemSet->bVisible = bVisibility;
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