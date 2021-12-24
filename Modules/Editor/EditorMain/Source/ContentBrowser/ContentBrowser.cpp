#include "ContentBrowser/ContentBrowser.h"
#include "File/File.h"
#include <filesystem>
#include "ContentBrowser/ContentBrowserWidget.h"
#include "Manager/IAssetManager.h"

namespace Ry
{
	ContentBrowser::ContentBrowser(Ry::SharedPtr<ContentBrowserWidget> Widget)
	{
		this->Browser = Widget;

		Widget->UpArrow->OnButtonReleased.AddMemberFunction(this, &ContentBrowser::UpDirectory);

		// Callback on assets imported
		Widget->ImportAssetsCallback.AddMemberFunction(this, &ContentBrowser::OnImportAssets);
	}

	void ContentBrowser::UpDirectory()
	{
		Ry::String Abs = Filesystem::canonical(*Ry::File::VirtualToAbsolute(CurrentDirectory)).string().c_str();
		Ry::String Parent = Filesystem::path(*Abs).parent_path().string().c_str();

		if(Ry::File::IsAbsPathUnderVirtual("Engine", Parent))
		{
			Ry::String ParentVirtual = Ry::File::AbsoluteToVirtual(Parent);

			SetDirectory(ParentVirtual);
		}
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
			bool bIsDirectory = Filesystem::is_directory(Path);
			bool bIsRAsset = Path.path().extension() == ".rasset";

			// Only show files of type .rasset
			//if (!bIsDirectory && !bIsRAsset)
				//continue;

			// Create a browser node for this
			BrowserNode Node;
			Node.Name = Path.path().stem().string().c_str();
			Node.bDirectory = Filesystem::is_directory(Path);

			if (Node.bDirectory)
			{
				Node.Widget = Browser->AddDirectory(Node.Name);
			}
			else
			{
				Node.Widget = Browser->AddFile(Node.Name);
			}

			if (Node.Widget)
			{
				Node.Widget->OnDoubleClick.AddMemberFunction(this, &ContentBrowser::OpenNode);
				Nodes.insert(Node.Widget.Get(), Node);
			}

		}

		Browser->MarkDirty(Browser.Get());
		
	}

	void ContentBrowser::OnImportAssets(const Ry::ArrayList<Ry::String>& AssetPaths)
	{
		std::cout << "Importing assets:" << std::endl;
		for(const Ry::String& Asset : AssetPaths)
		{
			std::cout << "\t" << *Asset << std::endl;
		}

		// Invoke the asset importer
		Ry::AssetMan->ImportAssets(CurrentDirectory, AssetPaths);

		// Refresh the current directory
		SetDirectory(CurrentDirectory);
	}

	void ContentBrowser::OpenNode(ContentBrowserItem* Item)
	{
		if(Nodes.contains(Item))
		{
			BrowserNode& Node = *Nodes.get(Item);

			if(Node.bDirectory)
			{
				// Concat paths, Go into directory
				Ry::String NewVirtual = CurrentDirectory + "/" + Node.Name;
				SetDirectory(NewVirtual);
			}
			else
			{
				// Asset open action
			}
		}
	}
	
}
