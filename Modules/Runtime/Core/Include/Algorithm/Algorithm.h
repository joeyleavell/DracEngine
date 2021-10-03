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

	template<typename Tag>
	struct TypeTag {};

	template <typename T>
	uint32 Hash(SharedPtr<T> Object)
	{
		return HashImpl<T>(TypeTag<T>{}, Object.Get());
	}

	// Generic hash impl, must be implemented
	template <typename T>
	uint32 HashImpl(TypeTag<T>, const T& Object);
	
	// Hash impl for pointer types
	template <typename T>
	uint32 HashImpl(TypeTag<T>, const T* Object)
	{
		return static_cast<uint32>(reinterpret_cast<uintptr_t>(Object));
	}

	// Hash imp for shared pointers

	template <>
	inline uint32 HashImpl<int32>(TypeTag<int32>, const int32& Object)
	{
		return Object;
	}

	template <>
	inline uint32 HashImpl<uint32>(TypeTag<uint32>, const uint32& Object)
	{
		return Object;
	}

	// Specialize hash function for string
	template <>
	inline uint32 HashImpl<Ry::String>(TypeTag<Ry::String>, const Ry::String& Object)
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

	template <typename T>
	uint32 Hash(const T* Object)
	{
		return (uint32) (Object);
	}

	template <typename T>
	uint32 Hash(T* Object)
	{
		return HashImpl<T>(TypeTag<T>{}, Object);
	}

	/**
	 * Overloaded hashing for object references
	 */
	template <typename T>
	uint32 Hash(const T& Object)
	{
		return HashImpl(TypeTag<T>{}, Object);
	}

}
