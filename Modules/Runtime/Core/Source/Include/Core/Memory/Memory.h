#pragma once

#include "Core/Platform.h"
#include "SharedPtr.h"
#include "UniquePtr.h"
#include <memory>
#include <cstring>
#include "CoreGen.h"

namespace Ry
{

	/**
	 * Smart pointers
	 */
	template <typename T>
	using UniquePtr = std::unique_ptr<T>;

	template <typename T>
	using WeakPtr = std::weak_ptr<T>;

	template<class T, class ...Args>
	Ry::UniquePtr<T> MakeUnique(Args... Arguments)
	{
		return std::make_unique<T>(Arguments...);
	}

	CORE_MODULE void MemCpy(void* Dest, uint64 DstSize, const void* Src, uint64 SrcSize);
	
}
