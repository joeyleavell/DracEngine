#pragma once

#include "Core/Core.h"
#include "AssetCoreGen.h"

namespace Ry
{
		
	class ASSETCORE_MODULE Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;

		template <class T>
		T* As()
		{
			return dynamic_cast<T*>(this);
		}

		/**
		 * Called when asset resources should be cleaned up.
		 * I.e. freeing texture resources, file contents, etc.
		 */
		virtual void UnloadAsset() = 0;

	};

	class ASSETCORE_MODULE AssetRef
	{
	public:
		AssetRef();
		AssetRef(const char* Data);
		AssetRef(const Ry::String& Path);
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
	
}
