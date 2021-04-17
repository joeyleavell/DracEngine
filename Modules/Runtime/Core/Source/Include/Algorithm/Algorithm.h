#pragma once

#include "Core/Core.h"
#include "Core/String.h"

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

	template <>
	inline CORE_MODULE uint32 Hash<int32>(const int32& Object)
	{
		return Object;
	}

	template <>
	inline CORE_MODULE uint32 Hash<uint32>(const uint32& Object)
	{
		return Object;
	}

	// Specialize hash function for string
	template <>
	inline CORE_MODULE uint32 Hash<Ry::String>(const Ry::String& Object)
	{
		const int32 p = 31;
		const int32 m = (uint32)1e9 + 9;
		uint32 hash_value = 0;
		uint32 p_pow = 1;

		for (uint32 Character = 0; Character < Object.getSize(); Character++)
		{
			char c = Object[Character];
			hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
			p_pow = (p_pow * p) % m;
		}

		return hash_value;
	}



}
