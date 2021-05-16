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
		SetDirectory("/Engine/");
	}

	void ContentBrowser::SetDirectory(Ry::String Virtual)
	{
		// Clear out nodes and widgets
		Nodes.Clear();
		Browser->ClearChildren();
		
		this->CurrentDirectory = Virtual;

		Browser->SetCurrentDirectory(Virtual);

		// Crawl files in this directory
		Ry::String Absolute = Ry::File::VirtualToAbsolute(Virtual);

		Filesystem::directory_iterator CurDirItr(*Absolute);

		// Iterate paths in directory
		for(auto& Path : CurDirItr)
		{
			// Create a browser node for this
			BrowserNode Node;
			Node.Name = Path.path().stem().string().c_str();
			Node.bDirectory = Filesystem::is_directory(Path);

			if(Node.bDirectory)
			{
				Node.Widget = Browser->AddDirectory(Node.Name);
			}
			else
			{
				Node.Widget = Browser->AddFile(Node.Name);
			}

			if(Node.Widget)
			{
				Node.Widget->OnDoubleClick.AddMemberFunction(this, &ContentBrowser::OpenNode);
				Nodes.insert(Node.Widget.Get(), Node);
			}
		}

		Browser->MarkDirty();
		
	}

	void ContentBrowser::OpenNode(ContentBrowserItem* Item)
	{
		if(Nodes.contains(Item))
		{
			BrowserNode& Node = *Nodes.get(Item);

			if(Node.bDirectory)
			{
				// Concat paths, Go into directory
				Ry::String NewVirtual = CurrentDirectory + "/" + Node.Name + "/";
				SetDirectory(NewVirtual);
			}
			else
			{
				// Asset open action
			}
		}
	}
	
}
