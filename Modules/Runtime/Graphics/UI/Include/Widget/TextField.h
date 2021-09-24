#pragma once

#include "Widget/Widget.h"
#include "Font.h"
#include "Color.h"
#include "Widget/HorizontalPanel.h"
#include "Style.h"
#include "Keys.h"

namespace Ry
{

	class UI_MODULE TextField : public Widget
	{
	public:

		WidgetBeginArgs(TextField)
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

		TextField() :
			Widget()
		{
			ItemSet = MakeItemSet();
			CursorItem = MakeItem();
			SelectionItem = MakeItem();

			CursorPos = 0;
			SelectionPos = -1;
		}

		SizeType ComputeSize() const override
		{
			if (bTextSizeDirty)
			{
				CachedSize.Width = static_cast<int32>(MaxSize.Width > 0 ? MaxSize.Width : Style.Font->MeasureWidth(Text));
				CachedSize.Height = static_cast<int32>(Style.Font->MeasureHeight(Text, static_cast<float>(CachedSize.Width)));
				bTextSizeDirty = false;
			}

			return CachedSize;
		}

		TextField& SetText(const Ry::String& Text)
		{
			this->Text = Text;
			bTextSizeDirty = true;

			if(CursorPos > Text.getSize())
			{
				CursorPos = Text.getSize();
			}

			// Pre compute text data
			ComputeTextData();
			MarkDirty();

			return *this;
		}

		TextField& SetStyle(BitmapFont* Font, const Color& Color)
		{
			Style.SetFont(Font).SetColor(Color);
			bTextSizeDirty = true;

			return *this;
		}

		TextField& SetFont(BitmapFont* Font)
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
			if (Bat)
			{
				Bat->AddItemSet(ItemSet, "Font", GetPipelineState(), Style.Font->GetAtlasTexture(), WidgetLayer + 1);

				Bat->AddItem(CursorItem, "Shape", GetPipelineState(), nullptr, WidgetLayer + 1);
			}

			if (CursorPos != SelectionPos && SelectionPos >= 0)
				ShowSelection();
		}

		void OnHide() override
		{
			if (Bat)
			{
				Bat->RemoveItemSet(ItemSet);

				Bat->RemoveItem(CursorItem);
			}

			HideSelection();
		}

		void Draw() override
		{
			if (IsVisible())
			{
				Point Abs = GetAbsolutePosition();
				Ry::BatchText(ItemSet, Style.TextColor, Style.Font, ComputedTextData, static_cast<float>(Abs.X), static_cast<float>(Abs.Y), static_cast<float>(ComputeSize().Width));

				Ry::ArrayList<float> XOffsets;
				Style.Font->MeasureXOffsets(XOffsets, Text);

				// Draw cursor
				float CursorX = XOffsets[CursorPos] + Abs.X;
				float Height = Style.Font->GetAscent() - Style.Font->GetDescent();
				Ry::BatchRectangle(CursorItem, WHITE, CursorX, Abs.Y, 1.0f, Height, 0.0f);

				// Draw selection if applicable
				if(SelectionPos >= 0)
				{
					float CursorX = XOffsets[CursorPos];
					float SelectionX = XOffsets[SelectionPos];
					float Width = std::abs(SelectionX - CursorX);
					float X = Abs.X + (CursorX < SelectionX ? CursorX : SelectionX);

					Ry::BatchRectangle(SelectionItem, BLUE, X, Abs.Y, Width, Height, 0.0f);
				}

			}
		}

		int32 FindClosestCursorIndex(int32 Offset)
		{
			int32 RetCursorPos = -1;
			
			// Calc offsets
			Ry::ArrayList<float> XOffsets;
			Style.Font->MeasureXOffsets(XOffsets, Text);

			// Find closest offset
			int32 SmallestDiff = INT32_MAX;
			int32 Index = 0;
			while (Index < XOffsets.GetSize())
			{
				int32 Delta = std::abs(Offset - XOffsets[Index]);
				if (Delta < SmallestDiff)
				{
					SmallestDiff = Delta;
					RetCursorPos = Index;
				}

				Index++;
			}

			return RetCursorPos;
		}

		bool OnMouseDragged(const MouseDragEvent& MouseEv) override
		{
			if(MouseEv.ButtonID == 0 && bDragging)
			{
				Point Abs = GetAbsolutePosition();
				int32 MouseXOffset = MouseEv.MouseX - Abs.X;
				CursorPos = FindClosestCursorIndex(MouseXOffset);

				std::cout << CursorPos << " " << SelectionPos << std::endl;

				Draw();
				Bat->Update();

				if(CursorPos != SelectionPos && SelectionPos >= 0)
				{
					ShowSelection();
				}
				else
				{
					HideSelection();
				}
			}

			return true;
		}

		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override
		{
			if (MouseEv.ButtonID == 0)
			{
				if (MouseEv.bPressed)
				{
					if (IsHovered())
					{
						bDragging = true;

						// Find closest offset
						Point Abs = GetAbsolutePosition();
						int32 MouseXOffset = MouseEv.MouseX - Abs.X;
						SelectionPos = FindClosestCursorIndex(MouseXOffset);
						CursorPos = SelectionPos;

					}
					else
					{
						SelectionPos = -1;
					}

					HideSelection();			
				}
				else
				{
					bDragging = false;
				}
			}
			
			Draw();
			Bat->Update();

			return true;
		}

		bool OnKey(const KeyEvent& KeyEv) override
		{
			if(KeyEv.Action == Ry::KeyAction::PRESS || KeyEv.Action == Ry::KeyAction::REPEAT)
			{
				if(KeyEv.KeyCode == KEY_BACKSPACE)
				{
					// Modify the text
					if(CursorPos > 0)
					{
						int32 Start;
						int32 End;
						if(CursorPos == SelectionPos || SelectionPos < 0)
						{
							Start = CursorPos - 1;
							End = CursorPos;
						}
						else
						{
							Start = CursorPos < SelectionPos ? CursorPos : SelectionPos;
							End = CursorPos < SelectionPos ? SelectionPos : CursorPos;
						}

						Ry::String Prev = Text.substring(0, Start);
						Ry::String Post;
						if (End <= Text.getSize() - 1)
						{
							Post = Text.substring(End);
						}

						if (CursorPos > Start)
						{
							CursorPos -= (End - Start);
						}
						SelectionPos = -1;
						
						HideSelection();
						
						SetText(Prev + Post);
					}
				}

				if(KeyEv.KeyCode == KEY_LEFT)
				{
					CursorPos--;
					if (CursorPos < 0)
						CursorPos = 0;
				}
				
				if (KeyEv.KeyCode == KEY_RIGHT)
				{
					CursorPos++;
					if (CursorPos > Text.getSize())
						CursorPos = Text.getSize();
				}

				std::cout << "cursor pos: " << CursorPos << std::endl;

				Draw();
				Bat->Update();
			}

			return true;
		}

		bool OnChar(const CharEvent& CharEv) override
		{
			// Modify the text
			if (Text.getSize() >= 1)
			{
				Ry::String Prev = Text.substring(0, CursorPos);
				Ry::String Post;
				if (CursorPos <= Text.getSize() - 1)
				{
					Post = Text.substring(CursorPos);
				}
				
				SetText(Prev + static_cast<char>(CharEv.Codepoint) + Post);
			}
			else
			{
				SetText(Ry::String("") + static_cast<char>(CharEv.Codepoint));
			}

			CursorPos++;

			Draw();
			Bat->Update();

			return true;
		}

	private:

		void ShowSelection()
		{
			if (Bat)
			{
				Bat->AddItem(SelectionItem, "Shape", GetPipelineState(), nullptr, WidgetLayer);
				//std::cout << "show sel" << std::endl;
			}
		}

		void HideSelection()
		{
			if(Bat)
			{
				Bat->RemoveItem(SelectionItem);
				std::cout << "hide sel" << std::endl;
			}
		}

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

		bool bDragging = false;
		bool bShowCursor;
		int32 CursorPos;
		int32 SelectionPos;

		mutable SizeType CachedSize;
		mutable bool bTextSizeDirty;
		PrecomputedTextData ComputedTextData;
		String Text;

		TextStyle Style;
		Ry::SharedPtr<BatchItemSet> ItemSet;

		Ry::SharedPtr<BatchItem> CursorItem;
		
		Ry::SharedPtr<BatchItem> SelectionItem;

	};
	
}
