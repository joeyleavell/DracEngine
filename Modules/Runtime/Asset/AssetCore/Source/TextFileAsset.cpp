#include "TextFileAsset.h"

namespace Ry
{

	TextFileAsset::TextFileAsset(const Ry::String& Content)
	{
		this->Contents = Content;
	}

	const Ry::String& TextFileAsset::GetContents() const
	{
		return Contents;
	}
	
	void TextFileAsset::UnloadAsset()
	{

	}
}
