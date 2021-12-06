#include "Package/Package.h"
#include "File/Serializer.h"
#include "Core/Globals.h"
#include "File/Deserializer.h"

namespace Ry
{

	Package::Package(const Ry::AssetRef& Location)
	{
		this->Location = Location;
		this->CachedObject = nullptr;
	}

	void Package::Save()
	{
		if(CachedObject)
		{
			Ry::Serializer Out;
			Out.Open(Location.GetAbsolute());
			{
				Out.WriteObject(CachedObject);
			}
			Out.Close();
		}
		else
		{
			Ry::Log->LogError("Object was not set prior to calling Package::Save()");
		}
	}

	void Package::SetObject(const Ry::Object* RootObject)
	{
		this->CachedObject = RootObject;
	}

	const Ry::Object* Package::GetOrLoadRoot()
	{
		if (!CachedObject)
		{
			Ry::Deserializer In;
			In.Open(Location.GetAbsolute());
			{
				CachedObject = In.ReadObject();
			}
			In.Close();

			// CachedObject is still null, means we failed to read an object. Print error.
			if(!CachedObject)
			{
				Ry::Log->LogErrorf("Failed to load root object from package %s resolved to %s", *Location.GetVirtual(), *Location.GetAbsolute());
			}
		}

		return CachedObject;
	}

	const Ry::AssetRef& Package::GetLocation() const
	{
		return Location;
	}

	Package* LoadPackage(const Ry::AssetRef& Ref)
	{
		// TODO: how to best manage packages?
		Package* NewPackage = new Package(Ref);
		return NewPackage;
	}

}
