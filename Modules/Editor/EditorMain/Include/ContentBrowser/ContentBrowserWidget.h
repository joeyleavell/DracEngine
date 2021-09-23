#pragma once

#include "Widget/SlotWidget.h"
#include "Widget/GridLayout.h"
#include "Widget/Label.h"
#include "Widget/BorderWidget.h"
#include "Widget/Button.h"
#include "Widget/ScrollPane.h"
#include "Widget/TextField.h"

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
		int32 kills = 0;

		Ry::SharedPtr<BorderWidget> Icon;
		Ry::SharedPtr<Label> Lab;

	};


	class ContentBrowserWidget : public SlotWidget
	{
	public:

		Ry::SharedPtr<Ry::ScrollPane> Pane;

		Ry::SharedPtr<TextField> CurDirLabel;
		Ry::SharedPtr<Button> UpArrow;

		ContentBrowserWidget();
		virtual ~ContentBrowserWidget();

		void SetCurrentDirectory(Ry::String Dir);
		Ry::SharedPtr<ContentBrowserItem> AddDirectory(Ry::String Name);
		Ry::SharedPtr<ContentBrowserItem> AddFile(Ry::String Name);

		void ClearChildren();

	private:

		BitmapFont* TextFont;
		Texture* DirectoryTexture;
		Texture* FileTexture;
		Texture* UpArrowTexture;

		Ry::SharedPtr<GridLayout> Grid;

	};

}
