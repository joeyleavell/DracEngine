#pragma once

#include "Widget/Widget.h"
#include "Font.h"
#include "Color.h"
#include "Widget/Layout/HorizontalPanel.h"
#include "Style.h"
#include "Keys.h"
#include "Core/PlatformProcess.h"
#include "TextField.gen.h"

namespace Ry
{

	class USERINTERFACE_MODULE TextField : public Widget
	{
	public:

		GeneratedBody()

		RefField()
		Ry::String TextStyleName;

		TextField();
		SizeType ComputeSize() const override;

		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;
		bool OnMouseDragged(const MouseDragEvent& MouseEv) override;
		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;

		bool OnKey(const KeyEvent& KeyEv) override;
		bool OnChar(const CharEvent& CharEv) override;
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void Draw() override;

		TextField& SetText(const Ry::String& Text);
		TextField& SetStyle(const Ry::String& TextStyleName);
		const Ry::String& GetText() const;

		int32 FindClosestCursorIndex(int32 Offset);

		bool IsDelimiter(char c);

		bool HasSelection();

		void RemoveSubstring(int32 Start, int32 End);

		void ChopSelection();

		int32 CursorAdvanceLeft(int32 Initial);
		int32 CursorAdvanceRight(int32 Initial);

		void HandleBackspace();
		void HandleLeftArrow(const KeyEvent& KeyEv);
		void HandleRightArrow(const KeyEvent& KeyEv);

		void InsertText(const Ry::String Insert);


	private:

		void UpdateSelectionBox();

		bool bDragging = false;
		int32 CursorPos;
		int32 SelectionPos;

		mutable SizeType CachedSize;
		mutable bool bTextSizeDirty;
		PrecomputedTextData ComputedTextData;
		String Text;

		Ry::SharedPtr<BatchItemSet> ItemSet;

		Ry::SharedPtr<BatchItem> CursorItem;
		
		Ry::SharedPtr<BatchItem> SelectionItem;

	} RefClass();
	
}
