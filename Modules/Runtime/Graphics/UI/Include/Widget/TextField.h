#pragma once

#include "Widget/Widget.h"
#include "Font.h"
#include "Color.h"
#include "Widget/HorizontalPanel.h"
#include "Style.h"
#include "Keys.h"
#include "Core/PlatformProcess.h"

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
			ComputeTextData(ComputedTextData, Text);
			MarkDirty(this);

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

			MarkDirty(this);
			return *this;
		}

		const Ry::String& GetText() const
		{
			return Text;
		}

		void OnShow(Ry::Batch* Batch) override
		{
			Batch->AddItemSet(ItemSet, "Font", GetPipelineState(this), Style.Font->GetAtlasTexture(), WidgetLayer + 1);
			Batch->AddItem(CursorItem, "Shape", GetPipelineState(this), nullptr, WidgetLayer + 1);

			if (CursorPos != SelectionPos && SelectionPos >= 0)
				Batch->AddItem(SelectionItem, "Shape", GetPipelineState(this), nullptr, WidgetLayer);
			
		}

		void OnHide(Ry::Batch* Batch) override
		{
			Batch->RemoveItemSet(ItemSet);
			Batch->RemoveItem(CursorItem);
			Batch->RemoveItem(SelectionItem);
		}

		void Draw(StyleSet* TheStyle) override
		{
			if (IsVisible())
			{
				Point Abs = GetAbsolutePosition();
				SizeType Size = ComputeSize();
				Ry::BatchText(ItemSet, Style.TextColor, Style.Font, ComputedTextData, static_cast<float>(Abs.X), static_cast<float>(Abs.Y + Size.Height), static_cast<float>(ComputeSize().Width));

				Ry::ArrayList<float> XOffsets;
				Style.Font->MeasureXOffsets(XOffsets, Text);

				// Draw cursor
				float CursorX = XOffsets[CursorPos] + Abs.X;
				float Height = (float) (Style.Font->GetAscent() - Style.Font->GetDescent());
				Ry::BatchRectangle(CursorItem, WHITE, CursorX, (float) (Abs.Y + Style.Font->GetDescent()), 1.0f, Height, 0.0f);

				// Draw selection if applicable
				if(SelectionPos >= 0)
				{
					float CursorX = XOffsets[CursorPos];
					float SelectionX = XOffsets[SelectionPos];
					float Width = std::abs(SelectionX - CursorX);
					float X = Abs.X + (CursorX < SelectionX ? CursorX : SelectionX);

					Ry::BatchRectangle(SelectionItem, WHITE.ScaleRGB(0.1f), X, (float) (Abs.Y + Style.Font->GetDescent()), Width, Height, 0.0f);
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
				int32 Delta = (int32) (std::abs(Offset - XOffsets[Index]));
				if (Delta < SmallestDiff)
				{
					SmallestDiff = Delta;
					RetCursorPos = Index;
				}

				Index++;
			}

			return RetCursorPos;
		}

		bool OnMouseClicked(const MouseClickEvent& MouseEv) override
		{
			if(MouseEv.ButtonID == 0 && MouseEv.bDoubleClick && IsHovered())
			{
				Point Abs = GetAbsolutePosition();
				int32 MouseXOffset = (int32) (MouseEv.MouseX - Abs.X);
				
				int32 Initial = FindClosestCursorIndex(MouseXOffset);
				CursorPos = CursorAdvanceRight(Initial);
				SelectionPos = CursorAdvanceLeft(Initial);

				MarkDirty(this);

				return true;
			}

			return false;
		}

		bool OnMouseDragged(const MouseDragEvent& MouseEv) override
		{
			if(MouseEv.ButtonID == 0 && bDragging)
			{
				Point Abs = GetAbsolutePosition();
				int32 MouseXOffset = (int32) (MouseEv.MouseX - Abs.X);
				CursorPos = FindClosestCursorIndex(MouseXOffset);

				MarkDirty(this);
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
						int32 MouseXOffset = (int32) (MouseEv.MouseX - Abs.X);
						SelectionPos = FindClosestCursorIndex(MouseXOffset);
						CursorPos = SelectionPos;

					}
					else
					{
						SelectionPos = -1;
					}

					MarkDirty(this);
				}
				else
				{
					bDragging = false;
				}
			}

			MarkDirty(this);

			return true;
		}

		bool IsDelimiter(char c)
		{
			if(c >= 65 && c <= 122) // alphabet
				return false;
			if (c >= 48 && c <= 57) // digits
				return false;

			return true;
		}

		bool HasSelection()
		{
			return SelectionPos >= 0 && SelectionPos != CursorPos;
		}

		void RemoveSubstring(int32 Start, int32 End)
		{
			Ry::String Prev = Text.substring(0, Start);
			Ry::String Post;
			if (End <= Text.getSize() - 1)
			{
				Post = Text.substring(End);
			}

			SetText(Prev + Post);

			if (CursorPos > Start)
			{
				CursorPos = Start;
			}
			
			SelectionPos = -1;
		}

		void ChopSelection()
		{
			// Modify the text
			if (SelectionPos >= 0)
			{
				int32 Start = CursorPos < SelectionPos ? CursorPos : SelectionPos;
				int32 End = CursorPos < SelectionPos ? SelectionPos : CursorPos;

				MarkDirty(this, true);
				RemoveSubstring(Start, End);
			}
		}

		void HandleBackspace()
		{
 			if(HasSelection())
			{
				ChopSelection();
			}
			else if(CursorPos > 0)
			{
				RemoveSubstring(CursorPos - 1, CursorPos);
			}
		}

		void HandleLeftArrow(const KeyEvent& KeyEv)
		{
			if (KeyEv.bCtrl && KeyEv.bShift)
			{
				int32 InitialCursor = CursorPos;
				CursorPos = CursorAdvanceLeft(InitialCursor);

				if (InitialCursor != CursorPos)
				{
					if (CursorPos == SelectionPos)
					{
						// We had a selection but it was broken
						SelectionPos = -1;
						MarkDirty(this);
					}
					else if (SelectionPos == -1 || InitialCursor == SelectionPos)
					{
						// We didn't have a selection, create one
						SelectionPos = InitialCursor;
						MarkDirty(this);
					}
				}


			}
			else if(KeyEv.bShift)
			{
				if (SelectionPos < 0 || SelectionPos == CursorPos)
				{
					SelectionPos = CursorPos;
					MarkDirty(this);
				}
				
				// Simply decrement cursor pos
				CursorPos--;
			}
			else
			{

				if (SelectionPos < 0)
				{
					CursorPos--;
				}
				else
				{
					// Left arrow takes smallest of cursor/selection pos
					CursorPos = CursorPos < SelectionPos ? CursorPos : SelectionPos;
					SelectionPos = -1;
				}

				MarkDirty(this);
			}

			if (CursorPos < 0)
				CursorPos = 0;
		}

		int32 CursorAdvanceLeft(int32 Initial)
		{
			int32 Increment = Initial;
			
			// Skip whitespace
			while (Increment > 0 && std::isspace(Text[Increment - 1]))
				Increment--;

			if (Increment > 0 && IsDelimiter(Text[Increment - 1]))
				Increment--;
			else
				// Move selection to the left (find next delimeter to the left)
				while (Increment > 0 && !IsDelimiter(Text[Increment - 1]))
					Increment--;

			Increment = std::max(Increment, 0);

			return Increment;
		}

		int32 CursorAdvanceRight(int32 Initial)
		{
			int32 Increment = Initial;
			// Skip whitespace
			while (Increment < Text.getSize() && std::isspace(Text[Increment]))
				Increment++;

			if (Increment < Text.getSize() && IsDelimiter(Text[Increment])) // Landed on a delimeter
				Increment++;
			else
				// Move selection to the right (find next delimeter to the left)
				while (Increment < Text.getSize() && !IsDelimiter(Text[Increment]))
					Increment++;
			
			Increment = std::min(Increment, (int32) Text.getSize());

			return Increment;
		}

		void HandleRightArrow(const KeyEvent& KeyEv)
		{
			if (KeyEv.bCtrl && KeyEv.bShift)
			{
				int32 InitialCursor = CursorPos;
				CursorPos = CursorAdvanceRight(InitialCursor);

				if (InitialCursor != CursorPos)
				{
					if (CursorPos == SelectionPos)
					{
						// We had a selection but it was broken
						SelectionPos = -1;
						MarkDirty(this);
					}
					else if (SelectionPos == -1 || InitialCursor == SelectionPos)
					{
						// We didn't have a selection, create one
						SelectionPos = InitialCursor;
						MarkDirty(this);
					}
				}
			}
			else if(KeyEv.bShift)
			{
				if (SelectionPos < 0 || SelectionPos == CursorPos)
				{
					SelectionPos = CursorPos;
					MarkDirty(this);
				}

				// Simply increment cursor pos
				CursorPos++;
			}
			else
			{
				if (SelectionPos < 0)
				{
					CursorPos++;
				}
				else
				{
					// Right arrow takes biggest of cursor/selection pos
					CursorPos = CursorPos < SelectionPos ? SelectionPos : CursorPos;
					SelectionPos = -1;
				}

				MarkDirty(this);
			}


			if (CursorPos > Text.getSize())
				CursorPos = Text.getSize();
		}

		void InsertText(const Ry::String Insert)
		{
			if (CursorPos != SelectionPos && SelectionPos >= 0)
				ChopSelection();

			if (Text.getSize() >= 1)
			{
				Ry::String Prev = Text.substring(0, CursorPos);
				Ry::String Post;
				if (CursorPos <= Text.getSize() - 1)
				{
					Post = Text.substring(CursorPos);
				}

				SetText(Prev + Insert + Post);
			}
			else
			{
				SetText(Insert);
			}

			CursorPos += Insert.getSize();
			SelectionPos = -1; // Selection immediately goes away on type
			MarkDirty(this);
		}

		bool OnKey(const KeyEvent& KeyEv) override
		{
			if(KeyEv.Action == Ry::KeyAction::PRESS || KeyEv.Action == Ry::KeyAction::REPEAT)
			{
				if(KeyEv.bCtrl)
				{
					// Copying text
					if (KeyEv.KeyCode == KEY_C && SelectionPos >= 0 && SelectionPos != CursorPos)
					{
						int32 Start = CursorPos > SelectionPos ? SelectionPos : CursorPos;
						int32 End = CursorPos > SelectionPos ? CursorPos : SelectionPos;
						Ry::String Substring = Text.substring(Start, End);
						Ry::SetClipboardString(Substring);
					}

					// Selecting all text
					if (KeyEv.KeyCode == KEY_A)
					{
						SelectionPos = 0;
						CursorPos = Text.getSize();

						MarkDirty(this);
					}

					// Pasting text
					if (KeyEv.KeyCode == KEY_V)
						InsertText(Ry::GetClipboardString());

				}


				if(KeyEv.KeyCode == KEY_BACKSPACE)
				{
					HandleBackspace();
				}

				if(KeyEv.KeyCode == KEY_LEFT)
				{
					HandleLeftArrow(KeyEv);
				}
				
				if (KeyEv.KeyCode == KEY_RIGHT)
				{
					HandleRightArrow(KeyEv);
				}

				MarkDirty(this);
			}

			return true;
		}

		bool OnChar(const CharEvent& CharEv) override
		{			
			// Modify the text
			InsertText(Ry::String("") + static_cast<char>(CharEv.Codepoint));

			MarkDirty(this, true);
			
			return true;
		}

	private:

		bool bDragging = false;
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
