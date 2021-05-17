#pragma once

#include "Widget/SlotWidget.h"
#include "Widget/GridLayout.h"
#include "Widget/Label.h"
#include "Widget/BorderWidget.h"
#include "Widget/Button.h"

namespace Ry
{

	class ContentBrowserItem : public SlotWidget
	{
	public:

		MulticastDelegate<ContentBrowserItem*> OnDoubleClick;

		ContentBrowserItem(Texture* Texture, BitmapFont* Font, Ry::String Name);
		virtual ~ContentBrowserItem();

		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;

	private:
		
		BorderWidget* Icon;

	};


	class ContentBrowserWidget : public SlotWidget
	{
	public:

		Label* CurDirLabel;
		Button* UpArrow;

		ContentBrowserWidget();
		virtual ~ContentBrowserWidget();

		void SetCurrentDirectory(Ry::String Dir);
		ContentBrowserItem* AddDirectory(Ry::String Name);
		ContentBrowserItem* AddFile(Ry::String Name);

		void ClearChildren();

	private:

		BitmapFont* TextFont;
		Texture* DirectoryTexture;
		Texture* FileTexture;
		Texture* UpArrowTexture;

		GridLayout* Grid;

	};

}
