#pragma once
#include "Widget/GridLayout.h"

namespace Ry
{
	class ContentBrowserWidget;

	class ContentBrowser
	{
	public:

		ContentBrowser(ContentBrowserWidget* Widget);

		void SetDirectory(Ry::String Virtual);

	private:

		Ry::String CurrentDirectory;

		GridLayout* Directory;

		ContentBrowserWidget* Browser;
	};
	
}
