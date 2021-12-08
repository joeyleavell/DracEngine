#pragma once
#pragma once
#include "Widget/Layout/GridPanel.h"
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

		void OnImportAssets(const Ry::ArrayList<Ry::String>& AssetPaths);

	private:

		void OpenNode(ContentBrowserItem* Item);

		Ry::String CurrentDirectory;

		Ry::Map<ContentBrowserItem*, BrowserNode> Nodes;
	};

}
