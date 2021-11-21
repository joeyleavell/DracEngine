#pragma once

#include "Widget/Layout/SlotWidget.h"
#include "Widget/Layout/GridPanel.h"
#include "Widget/Label.h"
#include "Widget/BorderWidget.h"
#include "Widget/Input/Button.h"
#include "Widget/Layout/ScrollPane.h"
#include "Widget/Input/TextField.h"

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

		Ry::SharedPtr<GridPanel> Grid;

	};

}
