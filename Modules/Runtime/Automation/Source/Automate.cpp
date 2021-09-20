#include "Automate.h"
#include "File/File.h"

namespace Ry
{
	void ReloadResourceDirectory(const Ry::String& SourceResources, const Ry::String& DstResources)
	{
		if (Ry::File::DoesFileExist(DstResources))
		{
			// Make file writable in case so we can hot reload assets
			Ry::File::MakeDirectoryWritable(DstResources);
		}

		Ry::File::CopyDirectory(SourceResources, DstResources);

		// TODO: Notify of a hot reload so assets can reload
	}
	
}
