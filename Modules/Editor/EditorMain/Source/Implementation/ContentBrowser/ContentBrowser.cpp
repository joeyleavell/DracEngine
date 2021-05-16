#include "ContentBrowser/ContentBrowser.h"
#include "File/File.h"
#include "ContentBrowser/ContentBrowserWidget.h"

namespace Ry
{
	ContentBrowser::ContentBrowser(ContentBrowserWidget* Widget)
	{
		this->Browser = Widget;

		// Add grid

		// Initialize content browser to a directory (virtual)
		// The virtual path will be translated to an absolute path for crawling
		SetDirectory("/Engine/Mesh/");
	}

	void ContentBrowser::SetDirectory(Ry::String Virtual)
	{
		this->CurrentDirectory = Virtual;

		// Crawl files in this directory
		Ry::String Absolute = Ry::File::VirtualToAbsolute(Virtual);

		Filesystem::directory_iterator CurDirItr(*Absolute);

		// Iterate paths in directory
		for(auto& Path : CurDirItr)
		{
			// Get name of item
			Ry::String Item = Path.path().stem().string().c_str();

			if(Filesystem::is_directory(Path))
			{
				Browser->AddDirectory(Item);
			}
			else
			{
				Browser->AddFile(Item);
			}
		}
		
	}
}
