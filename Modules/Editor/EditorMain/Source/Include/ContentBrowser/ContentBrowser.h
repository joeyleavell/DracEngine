#pragma once
#pragma once
#include "Widget/GridLayout.h"
#include "Data/Map.h"

namespace Ry
{
	class ContentBrowserItem;
	class ContentBrowserWidget;

	struct BrowserNode
	{
		SharedPtr<ContentBrowserItem> Widget;
		bool bDirectory = false;
		Ry::String Name;
	};

	class ContentBrowser
	{
	public:

		ContentBrowser(Ry::SharedPtr<ContentBrowserWidget> Widget);

		void SetDirectory(Ry::String Virtual);

		void UpDirectory();

		Ry::SharedPtr<ContentBrowserWidget> Browser;

	private:

		void OpenNode(ContentBrowserItem* Item);

		Ry::String CurrentDirectory;

		GridLayout* Directory;


		Ry::Map<ContentBrowserItem*, BrowserNode> Nodes;
	};

}
