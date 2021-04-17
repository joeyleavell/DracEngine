#pragma once

#include "String.h"
#include "Data/ArrayList.h"
#include "CoreGen.h"

#define REFLECT_PRIMITIVE(Name)template<> \
inline DataType CORE_MODULE GetTypeImpl<Name>(TypeTag<Name>) { return DataType{ #Name, sizeof(Name) }; };

namespace Ry
{

	template<typename T>
	struct TypeTag {};

	template<typename T>
	struct ClassTag {};

	class DataType
	{
	public:
		Ry::String Name;
		uint64 Size;

		DataType()
		{
			Size = 0;
			Name = "";
		}

		DataType(Ry::String Name, uint64 Size)
		{
			this->Name = Name;
			this->Size = Size;
		}
	};

	class Field
	{
	public:
		DataType Type;
		Ry::String Name;
		uint64 Offset;

		Field()
		{
			Name = "";
			Offset = 0;
		}
	};

	class Function
	{
	public:

		Field RetVal;
		Ry::ArrayList<Field> Parameters;

		Function()
		{
			
		}
		
	};

	class Class
	{
	public:

		Ry::String Name;

		Ry::ArrayList<Ry::Field> Fields;
		Ry::ArrayList<Ry::Function> Functions;

	};

	template<typename T>
	DataType GetTypeImpl(TypeTag<T>)
	{
		return DataType{ "none", 0 };
	}

	// Create GetTypeImpl for arrays lists, maps, sets, etc.
	
	template<typename T>
	DataType GetType()
	{
		return GetTypeImpl(TypeTag<T>{});
	};

	REFLECT_PRIMITIVE(uint8)
	REFLECT_PRIMITIVE(uint16)
	REFLECT_PRIMITIVE(uint32)
	REFLECT_PRIMITIVE(uint64)
	REFLECT_PRIMITIVE(int8)
	REFLECT_PRIMITIVE(int16)
	REFLECT_PRIMITIVE(int32)
	REFLECT_PRIMITIVE(int64)
	REFLECT_PRIMITIVE(float)
	REFLECT_PRIMITIVE(double)
	REFLECT_PRIMITIVE(char*)
	
}