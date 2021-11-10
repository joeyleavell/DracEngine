#pragma once

#include <map>
#include <vector>
#include <string>
#include "Core/Platform.h"

namespace nlohmann
{
	template<typename, typename>
	struct adl_serializer;
	
	template<template<typename U, typename V, typename... Args> class ObjectType,
		template<typename U, typename... Args> class ArrayType,
		class StringType, class BooleanType,
		class NumberIntegerType,
		class NumberUnsignedType,
		class NumberFloatType,
		template<typename U> class AllocatorType,
		template<typename T, typename SFINAE = void> class JSONSerializer,
		class BinaryType>
	class basic_json;

	typedef nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, double, std::allocator, nlohmann::adl_serializer, std::vector<std::uint8_t>> RyJson;
}

namespace Ry
{
	class String;

	class EXPORT_ONLY Json
	{

	public:

		Json();
		Json(double Value);
		Json(Ry::String Value);
		Json(uint64 Value);
		Json(int64 Value);
		Json(bool Value);
		Json(const Json& Other);
		Json(Json&& Other) noexcept;
		Json(nlohmann::RyJson* PimplCopy);
		virtual ~Json();

		// Type checking functions
		bool IsNull() const;
		bool IsObject() const;
		bool IsNumber() const;
		bool IsIntegerNumber() const;
		bool IsUnsignedNumber() const;
		bool IsFloatingPointNumber() const;
		bool IsArray() const;
		bool IsString() const;
		bool IsBoolean() const;

		int32 Num();

		// Functions that add an element to an array json
		template<typename T>
		void Add(const T& Value);

		template<typename T>
		T Value(Ry::String FieldName, T DefaultValue) const;

		template<typename T>
		T& operator[](int32 Index);

		Json operator[](Ry::String FieldName) const;

		template<typename T>
		void Insert(Ry::String FieldName, const T& Value);

		Ry::String Stringify() const;

		Json& operator=(const Json& Other);
		Json& operator=(Json&& Other) noexcept;

		static Json Parse(const Ry::String& Json);
		static Json CreateObject();
		static Json CreateArray();

		bool operator==(const Json& Other) const;

	private:

		nlohmann::RyJson* Pimpl;
	};

}
