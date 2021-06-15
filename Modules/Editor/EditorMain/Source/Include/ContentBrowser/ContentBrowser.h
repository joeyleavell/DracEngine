#pragma once
#include "Widget/GridLayout.h"
#include "Data/Map.h"

namespace Ry
{
	class ContentBrowserItem;
	class ContentBrowserWidget;

	struct BrowserNode
	{
		SharedPtr<ContentBrowserItem> Widget = nullptr;
		bool bDirectory = false;
		Ry::String Name;
	};

	class ContentBrowser
	{
	public:

		ContentBrowser(Ry::SharedPtr<ContentBrowserWidget> Widget);

		void SetDirectory(Ry::String Virtual);

		void UpDirectory();

	private:

		void OpenNode(ContentBrowserItem* Item);

		Ry::String CurrentDirectory;

		GridLayout* Directory;

		Ry::SharedPtr<ContentBrowserWidget> Browser;

		Ry::Map<ContentBrowserItem*, BrowserNode> Nodes;
	};

}
