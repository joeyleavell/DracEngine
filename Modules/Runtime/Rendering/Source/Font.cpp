#include "Font.h"
#include "Bitmap.h"
#include "Batch.h"
#include "Core/Globals.h"
#include <iostream>
#include "RenderAPI.h"
#include "Texture.h"
#include "Bitmap.h"

namespace Ry
{

	BitmapFont::BitmapFont():
	Ascent(0),
	Descent(0),
	LineGap(0),
	MinCodepoint(0),
	MaxCodepoint(0),
	AtlasTexture(nullptr)
	{
		
	}

	BitmapFont::~BitmapFont()
	{
		// Delete the rasters for this font
		KeyIterator<int32, Ry::BitmapGlyph*> KeyItr = RegisteredGlyphs.CreateKeyIterator();

		while(KeyItr)
		{
			Ry::BitmapGlyph* NextGlyph = *KeyItr.Value();

			// Free bitmap glyph resources
			delete NextGlyph->Raster;
			delete NextGlyph;
			
			++KeyItr;
		}
	}

	int32 BitmapFont::GetAscent() const
	{
		return Ascent;
	}

	int32 BitmapFont::GetDescent() const
	{
		return Descent;
	}

	int32 BitmapFont::GetLineGap() const
	{
		return LineGap;
	}

	int32 BitmapFont::GetMinCodepoint() const
	{
		return MinCodepoint;
	}

	int32 BitmapFont::GetMaxCodepoint() const
	{
		return MaxCodepoint;
	}

	BitmapGlyph* BitmapFont::GetGlyph(int32 Codepoint) const
	{
		BitmapGlyph** Glyph = RegisteredGlyphs.get(Codepoint);
		if (Glyph)
		{
			return *Glyph;
		}
		return nullptr;
	}

	Ry::Texture* BitmapFont::GetAtlasTexture() const
	{
		return AtlasTexture;
	}

	float BitmapFont::MeasureHeight(const Ry::String& Text, float WrapWidth)
	{
		if (MeasuredHeight.Contains(Text))
			return MeasuredHeight.Get(Text);

		// The amount of spaces in a tab
		const int32 TAB_SPACES = 4;
		const int32 SPACE_ADVANCE = GetGlyph(static_cast<int32>(' '))->AdvanceWidth;

		// Establish origin
		float CurX = 0.0f;
		float CurY = static_cast<float>(GetAscent());

		// Get the advance width of the space character

		StringView* Lines = nullptr;
		int32 LineCount = Text.split("\n", &Lines);

		for (int32 Line = 0; Line < LineCount; Line++)
		{
			StringView* Words = nullptr;
			int32 WordCount = Lines[Line].split(" ", &Words);

			for (int32 Word = 0; Word < WordCount; Word++)
			{
				const Ry::StringView& CurrentWord = Words[Word];
				float TextWidth = MeasureWidth(CurrentWord);

				// Wrap text to next line
				if (CurX + TextWidth > WrapWidth)
				{
					// Reset cursor
					CurX = 0.0f;
					CurY += GetAscent() - GetDescent() + GetLineGap();
				}

				for (uint32 WordChar = 0; WordChar < CurrentWord.getSize(); WordChar++)
				{
					char Character = CurrentWord[WordChar];
					int32 Codepoint = static_cast<int32>(Character);
					BitmapGlyph* BmpGlyph = GetGlyph(Codepoint);

					// Generate texture coordinates for the glyph
					if (!BmpGlyph)
					{
						if (Codepoint == '\t')
						{
							CurX += TAB_SPACES * SPACE_ADVANCE;
						}
					}
					else
					{
						// Advance the cursor
						CurX += BmpGlyph->AdvanceWidth;

						// Kerning lookup
						if (WordChar < CurrentWord.getSize() - 1)
						{
							int32 NextCodepoint = static_cast<int32>(CurrentWord[WordChar + 1]);
							if (GetGlyph(NextCodepoint))
							{
								int32 KernAmount = *BmpGlyph->KerningTable.get(NextCodepoint);
								CurX += KernAmount;
							}
						}

					}

				}

				if (Word < WordCount - 1)
				{
					CurX += SPACE_ADVANCE;
				}
			}

			// Move cursor to the next line
			if(Line < LineCount - 1)
			{
				CurY += GetAscent() - GetDescent() + GetLineGap();
				CurX = 0.0f;
			}
		}

		delete[] Lines;

		MeasuredHeight.Insert(Text, CurY);

		return CurY;
	}
	
	float BitmapFont::MeasureWidth(const Ry::String& String)
	{
		// todo: get rid of need to cache width/height - make this function faster
		if (MeasuredWidth.Contains(String))
			return MeasuredWidth.Get(String);
		
		const int32 TAB_SPACES = 4;

		Ry::StringView* Lines = nullptr;

		// TODO: optimize string split? having to manually delete output might be a bad idea
		int32 LineCount = String.split("\n", &Lines);

		int32 MaxLineWidth = 0;

		for (int32 LineIndex = 0; LineIndex < LineCount; LineIndex++)
		{
			int32 CursorPosition = 0;
			int32 LineWidth = 0;
			Ry::StringView& Line = Lines[LineIndex];

			for (uint32 Char = 0; Char < Line.getSize(); Char++)
			{
				int32 Codepoint = static_cast<int32>(Line[Char]);
				BitmapGlyph* Glyph = GetGlyph(Codepoint);

				if (!Glyph)
				{

					// Add spacing for tabs
					if (Codepoint == '\t')
					{
						Glyph = GetGlyph(' ');
						CursorPosition += TAB_SPACES * Glyph->AdvanceWidth;
					}

				}
				else
				{
					// Adjust for left side bearing only if this is not the first word of the line 
					int32 CharacterReach = CursorPosition + (Glyph->Raster->GetWidth() + Glyph->OriginX);

					if (CharacterReach > LineWidth)
					{
						LineWidth = CharacterReach;
					}

					// Move pen to next character
					CursorPosition += Glyph->AdvanceWidth;

					// Kerning
					if (Char < Line.getSize() - 1)
					{
						int32 NextCodepoint = static_cast<int32>(Line[Char + 1]);

						if (Glyph->KerningTable.contains(NextCodepoint))
						{
							CursorPosition += *Glyph->KerningTable.get(NextCodepoint);
						}
					}

				}
			}

			if (LineWidth > MaxLineWidth)
			{
				MaxLineWidth = LineWidth;
			}
		}

		delete[] Lines;

		MeasuredWidth.Insert(String, static_cast<float>(MaxLineWidth));

		return (float) MaxLineWidth;
	}

	void BitmapFont::MeasureXOffsets(Ry::ArrayList<float>& OutOffsets, const Ry::String& String)
	{
		const int32 TAB_SPACES = 4;
		const int32 SPACE_ADVANCE = GetGlyph(' ')->AdvanceWidth;

		int32 CursorPosition = 0;

		for (uint32 Char = 0; Char <= String.getSize(); Char++)
		{
			int32 Codepoint = static_cast<int32>(String[Char]);
			BitmapGlyph* Glyph = GetGlyph(Codepoint);

			OutOffsets.Add((float) CursorPosition);

			if (!Glyph)
			{
				// Add spacing for tabs
				if (Codepoint == '\t')
					CursorPosition += TAB_SPACES * SPACE_ADVANCE;
			}
			else
			{
				// Move pen to next character
				CursorPosition += Glyph->AdvanceWidth;

				// Apply kerning specific to the next character
				if (Char < String.getSize() - 1)
				{
					int32 NextCodepoint = static_cast<int32>(String[Char + 1]);

					if (Glyph->KerningTable.contains(NextCodepoint))
					{
						CursorPosition += *Glyph->KerningTable.get(NextCodepoint);
					}
				}
			}
		}

	}

	void BitmapFont::SetAscent(int32 Ascent)
	{
		this->Ascent = Ascent;
	}

	void BitmapFont::SetDescent(int32 Descent)
	{
		this->Descent = Descent;
	}

	void BitmapFont::SetLineGap(int32 LineGap)
	{
		this->LineGap = LineGap;
	}

	void BitmapFont::SetCodepointRange(int32 MinCodepoint, int32 MaxCodepoint)
	{
		this->MinCodepoint = MinCodepoint;
		this->MaxCodepoint = MaxCodepoint;
	}

	void BitmapFont::AddGlyph(int32 Codepoint, BitmapGlyph* Glyph)
	{
		RegisteredGlyphs.insert(Codepoint, Glyph);
	}

	void BitmapFont::RemoveGlyph(int32 Codepoint)
	{
		RegisteredGlyphs.remove(Codepoint);
	}
	
	void BitmapFont::CreateRenderingResources()
	{		
		const int32 LINE_HEIGHT = Ascent - Descent + LineGap;
		auto* FontAtlas = new Ry::Bitmap(FONT_BITMAP_SIZE, FONT_BITMAP_SIZE, PixelStorage::RED8);
		
		AtlasTexture = Ry::RendAPI->CreateTexture(TextureFiltering::Linear);

		int32 X = 0;
		int32 Y = 0;
		int32 Codepoint = MinCodepoint;

		while(Codepoint <= MaxCodepoint)
		{
			Ry::BitmapGlyph* Next = *RegisteredGlyphs.get(Codepoint);

			// Check if we need to advance to the next line
			if(X + Next->Raster->GetWidth() >= FONT_BITMAP_SIZE)
			{
				Y += LINE_HEIGHT;
				X = 0;
				
				if(Y + Descent >= FONT_BITMAP_SIZE)
				{
					std::cerr << "Ran out of room on texture while generating font, not all characters will be available. Last codepoint was " << Codepoint << std::endl;
					break;
				}
			}

			// Place the coordinate information into the bitmap glyph
			Next->SheetX = X;
			Next->SheetY = Y;

			// Render the glyph to the font texture
			FontAtlas->DrawTexture(Next->Raster, X, Y);
			// Bitmap->DrawBox(X, Y, Next->Raster->GetWidth(), Next->Raster->GetHeight(), Ry::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

			X += Next->Raster->GetWidth();

			Codepoint++;
		}

		// Send the rendered glyph sheet to the rendering hardware
		AtlasTexture->Data(reinterpret_cast<uint8*>(FontAtlas->GetPixelBuffer()->GetAsRGBA()), FontAtlas->GetWidth(), FontAtlas->GetHeight(), PixelFormat::R8G8B8A8);

		// Delete the intermediate bitmap atlas
		delete FontAtlas;
	}

	void BitmapFont::DeleteRenderingResources()
	{
		// Free rendering resources
		delete AtlasTexture;
	}


}
