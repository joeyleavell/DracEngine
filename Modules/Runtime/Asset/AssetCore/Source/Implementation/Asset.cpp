#include "Asset.h"
#include "File/File.h"
#include "Algorithm/Algorithm.h"

namespace Ry
{

	AssetRef::AssetRef()
	{
		this->Path = "";
	}

	AssetRef::AssetRef(const char* Data)
	{
		this->Path = Data;
	}
	
	AssetRef::AssetRef(const Ry::String& Path)
	{
		this->Path = Path;
	}
	
	AssetRef::AssetRef(const AssetRef& Other)
	{
		this->Path = Other.Path;
	}

	AssetRef& AssetRef::operator=(const AssetRef& Other)
	{
		if (this == &Other)
			return *this;
		
		this->Path = Other.Path;

		return *this;
	}

	bool AssetRef::operator==(const AssetRef& Other) const
	{
		return GetAbsolute() == Other.GetAbsolute();
	}

	const char* AssetRef::operator*() const
	{
		return *Path;
	}

	bool AssetRef::Exists() const
	{
		return Ry::File::DoesFileExist(Ry::File::VirtualToAbsolute(Path));
	}

	Ry::String AssetRef::GetAbsolute() const
	{
		return Ry::File::VirtualToAbsolute(Path);
	}
	
	Ry::String AssetRef::GetVirtual() const
	{
		return Path;
	}

	
}
