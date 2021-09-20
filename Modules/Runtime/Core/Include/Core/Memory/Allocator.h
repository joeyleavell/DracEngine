#pragma once

#include "Core/Core.h"
#include "Allocator.gen.h"

namespace Ry
{

	/**
	 * Interface that defines an abstract memory allocator.
	 * 
	 */
	class CORE_MODULE Allocator
	{
	public:

		Allocator() {};
		virtual ~Allocator() {};

		template <typename T>
		T* Alloc(int32 Count = 0)
		{
			void* Chunk = Allocate(sizeof(T) * Count);
			return static_cast<T*>(Chunk);
		}

		virtual void* Allocate(int32 Size = 0) = 0;
		virtual void Free(void* Chunk) = 0;
		
	};

}