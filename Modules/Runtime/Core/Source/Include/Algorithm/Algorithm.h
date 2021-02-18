#pragma once

#include "Core/Core.h"

namespace Ry
{
	template<class T>
	class Iterator
	{
	public:
		Iterator() = default;
		virtual ~Iterator() = default;
		
		virtual explicit operator bool() const = 0;
		virtual T* operator*() const = 0;
		virtual Iterator& operator++() = 0;
	};

	/**
	 * Overloaded hashing for object references
	 */
	template <typename T>
	uint32 Hash(const T& Object);


}
