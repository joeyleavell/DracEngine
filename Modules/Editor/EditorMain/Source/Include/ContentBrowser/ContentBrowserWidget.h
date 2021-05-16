#pragma once

#include "Widget/SlotWidget.h"
#include "Widget/GridLayout.h"

namespace Ry
{

	class ContentBrowserWidget : public SlotWidget
	{
	public:

		ContentBrowserWidget();
		virtual ~ContentBrowserWidget();

		void AddDirectory(Ry::String Name);
		void AddFile(Ry::String Name);

	private:

		BitmapFont* TextFont;
		Texture* DirectoryTexture;
		Texture* FileTexture;

		GridLayout* Grid;

	};

}
