#pragma once

#include "Core/Core.h"
#include "Core/Object.h"
#include "Asset.gen.h"

namespace Ry
{

	class ASSETCORE_MODULE NewAsset : public Ry::Object
	{
	public:

		GeneratedBody()

		/**
		 * Called when asset resources should be cleaned up.
		 * I.e. freeing texture resources, file contents, etc.
		 */
		virtual void UnloadAsset() = 0;

	} RefClass();
		
	class ASSETCORE_MODULE Asset
	{
	public:

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
	
}
