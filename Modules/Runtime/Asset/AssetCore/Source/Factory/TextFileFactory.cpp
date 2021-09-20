#include "Factory/TextFileFactory.h"
#include "TextFileAsset.h"
#include "File/File.h"

namespace Ry
{
	void TextFileFactory::LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst)
	{
		AssetDst.push_back(new TextFileAsset(Ry::File::LoadFileAsString2(Reference.GetAbsolute())));
	}
}
