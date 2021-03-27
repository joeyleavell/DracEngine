#pragma once

#include "String.h"
#include "Data/ArrayList.h"
#include "CoreGen.h"

#define REFLECT_PRIMITIVE(Name)template<> \
inline DataType CORE_MODULE GetType<Name>() { return DataType{ #Name, sizeof(Name) }; };

namespace Ry
{

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
		DataType* Type;
		Ry::String Name;
		uint64 Offset;

		Field()
		{
			Type = nullptr;
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

		Ry::ArrayList<Ry::Field> Fields;
		Ry::ArrayList<Ry::Function> Functions;

	};

	template<typename T>
	DataType GetType() { return DataType{ "none", 0 }; };

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