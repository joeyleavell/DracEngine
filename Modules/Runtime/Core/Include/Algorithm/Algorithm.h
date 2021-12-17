#pragma once

#include "Core/Memory/SharedPtr.h"
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

	// The default hash implementation
	template<typename T>
	struct EXPORT_ONLY Hash
	{
		uint32 operator()(const T* Input) const
		{
			return static_cast<uint32>(reinterpret_cast<uintptr_t>(Input));			
		}

		uint32 operator()(const T& Input) const
		{
			return 0;
		}		
	};

	template<typename T>
	struct EXPORT_ONLY Hash<SharedPtr<T>>
	{
		uint32 operator()(const SharedPtr<T> Ptr) const
		{
			Ry::Hash<T> HashFunctor;
			return HashFunctor(Ptr.Get());
		}		
	};

	template<>
	struct EXPORT_ONLY Hash<uint32>
	{
		uint32 operator()(const uint32& Val) const
		{
			return Val;
		}		
	};

	template<>
	struct EXPORT_ONLY Hash<int32>
	{
		uint32 operator()(const int32& Val) const
		{
			return Val;
		}		
	};

	template<>
	struct EXPORT_ONLY Hash<Ry::String>
	{
		uint32 operator()(const Ry::String& Str) const
		{
			const int32 p = 31;
			const int32 m = (uint32)1e9 + 9;
			uint32 hash_value = 0;
			uint32 p_pow = 1;

			for (uint32 Character = 0; Character < Str.getSize(); Character++)
			{
				char c = Str[Character];
				hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
				p_pow = (p_pow * p) % m;
			}

			return hash_value;
		}
	};

//	template<typename Tag>
//	struct HashTypeTag {};

	// Generic hash impl, must be implemented
/*	template <typename T>
	uint32 HashImpl(HashTypeTag <T>, const T& Object)
	{
		std::cerr << "ERROR: generic hash impl used" << std::endl;
		return 0;
	};*/

	// template <typename T>
	// EXPORT_ONLY uint32 Hash(SharedPtr<T> Object)
	// {
	// 	return HashImpl<T>(HashTypeTag<T>{}, Object.Get());
	// }
	
	// Hash impl for pointer types
	// template <typename T>
	// EXPORT_ONLY uint32 HashImpl(HashTypeTag <T>, const T* Object)
	// {
	// 	return static_cast<uint32>(reinterpret_cast<uintptr_t>(Object));
	// }

	// Hash imp for shared pointers

	// template <>
	// EXPORT_ONLY inline uint32 HashImpl<int32>(HashTypeTag <int32>, const int32& Object)
	// {
	// 	return Object;
	// }

	// template <>
	// EXPORT_ONLY inline uint32 HashImpl<uint32>(HashTypeTag <uint32>, const uint32& Object)
	// {
	// 	return Object;
	// }

	// Specialize hash function for string
	// template <>
	// EXPORT_ONLY inline uint32 HashImpl<Ry::String>(HashTypeTag <Ry::String>, const Ry::String& Object)
	// {
	// }

	// template <typename T>
	// EXPORT_ONLY uint32 Hash(const T* Object)
	// {
	// 	return (uint32) (Object);
	// }

	// template <typename T>
	// EXPORT_ONLY uint32 Hash(T* Object)
	// {
	// 	return HashImpl<T>(HashTypeTag<T>{}, Object);
	// }

	/**
	 * Overloaded hashing for object references
	 */
	// template <typename T>
	// EXPORT_ONLY uint32 Hash(const T& Object)
	// {
	// 	return HashImpl(HashTypeTag<T>{}, Object);
	// }

}
