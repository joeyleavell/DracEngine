#pragma once

#include "AssetRef.gen.h"

namespace Ry
{
	class CORE_MODULE AssetRef
	{
	public:
		AssetRef();
		AssetRef(const char* Data);
		AssetRef(const Ry::String& Path);
		AssetRef(Ry::String&& Path);
		AssetRef(const AssetRef& Other);

		AssetRef& operator=(const AssetRef& Other);

		bool operator==(const AssetRef& Other) const;
		// uint32 operator()() const;

		const char* operator*() const;

		bool Exists() const;
		Ry::String GetAbsolute() const;
		Ry::String GetVirtual() const;

	private:
		Ry::String Path;
	};

	template<>
	struct EXPORT_ONLY Hash<Ry::AssetRef>
	{
		uint32 operator()(const Ry::AssetRef& Object) const
		{
			Ry::Hash<Ry::String> HashFunctor;
			return HashFunctor(Object.GetAbsolute()); // Hash the absolute because Engine/ and /Engine/ should map to the same entry
		}
	};

}
