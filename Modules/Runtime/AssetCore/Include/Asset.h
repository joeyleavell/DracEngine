#pragma once

#include "Core/Core.h"
#include "AssetCoreGen.h"
#include "Algorithm/Algorithm.h"

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
	
}
