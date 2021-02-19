#pragma once

#include "Core/Core.h"
#include "Core/String.h"

namespace Ry
{
	/*
	class RENDERING_MODULE Text
	{
	public:

		Text()
		{
			this->Lines = nullptr;
			this->LineCount = 0;
		}

		Text(const Ry::String& TextBody):
		Lines(nullptr),
		LineCount(0)
		{
			SetText(TextBody);
		}

		const Ry::String& GetFullString() const
		{
			return FullString;
		}

		Ry::StringView& GetWord(int32 Line, int32 Word) const
		{
			return *Lines[Line].Words[Word].Content.get();
		}

		int32 GetLineCount() const
		{
			return LineCount;
		}

		int32 GetLineWordCount(int32 Line) const
		{
			return Lines[Line].WordCount;
		}

		int32 GetWordWidth(int32 Line, int32 Word) const
		{
			return Lines[Line].Words[Word].Width;
		}

		int32 GetTextWidth() const
		{
			return TextWidth;
		}

		int32 GetTextHeight() const
		{
			return TextHeight;
		}

		void SetText(const Ry::String& TextBody)
		{
			this->FullString = TextBody;

			// Split the string into lines of text
			SharedPtr<StringView[]> TextLines;
			LineCount = FullString.split("\n", TextLines);

			// Create the lines
			auto* CreatedLines = new Line[LineCount];

			for (int32 Line = 0; Line < LineCount; Line++)
			{
				StringView* LineWords;
				StringView& TextLine = TextLines[Line];
				Text::Line& CurrentLine = CreatedLines[Line];

				CurrentLine.WordCount = TextLine->split(" ", &LineWords);
				Word* ResultWords = new Word[CurrentLine.WordCount];

				for (int32 Word = 0; Word < CurrentLine.WordCount; Word++)
				{
					ResultWords[Word].Content.reset(LineWords + Word);
					ResultWords->Width = 0;
				}

				CurrentLine.Words.reset(ResultWords);
			}

			Lines.reset(CreatedLines);
		}

		void SetTextWidth(int32 TextWidth)
		{
			this->TextWidth = TextWidth;
		}

		void SetTextHeight(int32 TextHeight)
		{
			this->TextHeight = TextHeight;
		}

		void SetWordWidth(int32 Line, int32 Word, int32 Width)
		{
			Lines[Line].Words[Word].Width = Width;
		}

	private:

		struct Word
		{
			Ry::UniquePtr<Ry::StringView> Content;
			int32 Width;
		};

		struct Line
		{
			Ry::UniquePtr<Word[]> Words;
			int32 WordCount;
		};

		int32 TextWidth;
		int32 TextHeight;
		Ry::String FullString;
		int32 LineCount;
		Ry::UniquePtr<Line[]> Lines;

	};
	*/
}
