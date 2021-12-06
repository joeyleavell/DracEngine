#pragma once

#include "Core/Object.h"
#include "File/AssetRef.h"
#include "Package.gen.h"

namespace Ry
{

	class CORE_MODULE Package
	{
	public:

		Package(const Ry::AssetRef& Ref);

		/**
		 * Writes out the data for this package to the drive.
		 */
		void Save();

		/**
		 * Sets the object to be saved. If this package's root object has already been resolved, the cached object is overwritten by the specified root object.
		 */
		void SetObject(const Ry::Object* RootObject);

		/**
		 * Loads the root object if it hasn't already been loaded.
		 */
		const Ry::Object* GetOrLoadRoot();

		/**
		 * Gets the physical location of this package.
		 */
		const Ry::AssetRef& GetLocation() const;

	private:

		const Ry::Object* CachedObject;
		Ry::AssetRef Location;

	};

	/**
	 * Loads a package at the specified asset path.
	 */
	CORE_MODULE Package* LoadPackage(const Ry::AssetRef& Ref);

}
