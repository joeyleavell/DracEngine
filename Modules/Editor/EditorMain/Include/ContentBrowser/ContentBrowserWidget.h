#pragma once

#include "Widget/Layout/SlotWidget.h"
#include "Widget/Layout/GridPanel.h"
#include "Widget/Label.h"
#include "Widget/BorderWidget.h"
#include "Widget/Input/Button.h"
#include "Widget/Layout/ScrollPane.h"
#include "Widget/Input/TextField.h"
#include "ContentBrowserWidget.gen.h"

namespace Ry
{

	class ContentBrowserItem : public SlotWidget
	{
	public:

		MulticastDelegate<ContentBrowserItem*> OnDoubleClick;

		ContentBrowserItem(Ry::String TileIconStyle, Ry::String Name);
		virtual ~ContentBrowserItem();

		void Construct();

		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;

	private:

		Ry::SharedPtr<BorderWidget> Icon;
		Ry::SharedPtr<Label> Lab;

		Ry::String Name;
		Ry::String TileIconStyle;

	};

	class ContentBrowserWidget : public SlotWidget
	{
	public:

		GeneratedBody()

		Ry::SharedPtr<TextField> CurDirLabel;
		Ry::SharedPtr<Button> UpArrow;

		ContentBrowserWidget();
		virtual ~ContentBrowserWidget();

		void SetCurrentDirectory(Ry::String Dir);
		Ry::SharedPtr<ContentBrowserItem> AddDirectory(Ry::String Name);
		Ry::SharedPtr<ContentBrowserItem> AddFile(Ry::String Name);

		void ClearChildren();

	private:

		Ry::SharedPtr<GridPanel> Grid;
	} RefClass();

}
