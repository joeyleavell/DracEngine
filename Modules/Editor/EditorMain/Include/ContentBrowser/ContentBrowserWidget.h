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

		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;
		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;
		bool OnMouseDragged(const MouseDragEvent& MouseEv) override;

		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void Draw() override;

	private:

		void ShowGhost(Batch* Bat);
		void HideGhost(Batch* Bat);

		Point Offset;
		bool bIsPressed;
		bool bIsDragging;
		bool bGhostShown;

		Ry::SharedPtr<BorderWidget> Icon;
		Ry::SharedPtr<Label> Lab;
		Ry::SharedPtr<Ry::BatchItemSet> GhostIconItemSet;

		Ry::String Name;
		Ry::String TileIconStyle;

		float LastMouseX;
		float LastMouseY;

	};

	class ContentBrowserWidget : public SlotWidget
	{
	public:

		GeneratedBody()

		MulticastDelegate<const Ry::ArrayList<Ry::String>&> ImportAssetsCallback;

		Ry::SharedPtr<TextField> CurDirLabel;
		Ry::SharedPtr<Button> UpArrow;

		ContentBrowserWidget();
		virtual ~ContentBrowserWidget();

		void SetCurrentDirectory(Ry::String Dir);
		Ry::SharedPtr<ContentBrowserItem> AddDirectory(Ry::String Name);
		Ry::SharedPtr<ContentBrowserItem> AddFile(Ry::String Name);

		void ClearChildren();

		bool OnPathDrop(const PathDropEvent& PathDropEv) override;

	private:

		Ry::SharedPtr<GridPanel> Grid;
	} RefClass();

}
