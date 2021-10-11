#pragma once

#include "String.h"
#include "Data/ArrayList.h"
#include "Data/Map.h"
#include "CoreGen.h"

#define REFLECT_PRIMITIVE(InTypeName, InTypeClass) \
template<> \
inline const Ry::DataType* GetTypeImpl<InTypeName>(Ry::TypeTag<InTypeName>) \
{ \
	static Ry::DataType Result; \
	Result.Size = sizeof(InTypeName); \
	Result.Name = #InTypeName; \
	Result.Class = InTypeClass; \
	return &Result; \
};

namespace Ry
{

	class ReflectedClass;

	template<typename T>
	struct TypeTag {};

	template<typename T>
	struct ClassTag {};

	enum class TypeClass
	{
		UInt8,
		UInt16,
		UInt32,
		UInt64,
		Int8,
		Int16,
		Int32,
		Int64,
		Bool,
		Float,
		Double,

		ArrayList,
		Object
	};
	
	class Property
	{
		Ry::String Name;
		Ry::String Type;


	};

	class DataType
	{
	public:
		Ry::TypeClass Class;
		Ry::String Name;
		uint64 Size;

		bool IsObject()
		{
			return Class == TypeClass::Object;
		}

		bool IsFloating()
		{
			switch (Class)
			{
			case TypeClass::Double:
			case TypeClass::Float:
				return true;
			default:
				return false;
			}
		}

		bool IsInteger()
		{
			switch (Class)
			{
			case TypeClass::ArrayList:
			case TypeClass::Object:
			case TypeClass::Double:
			case TypeClass::Float:
				return false;
			default:
				return true;
			}
		}

	};

	class Field
	{
	public:
		const DataType* Type;
		Ry::String Name;
		uint64 Offset;
		const Ry::ReflectedClass* ObjectClass; // If this type is an object, this value represents the class of he object

		// Only applicable for ArrayLists at the moment
		Ry::ArrayList<DataType> TemplateType;

		Field()
		{
			Name = "";
			Offset = 0;
			this->Type = nullptr;
			this->ObjectClass = nullptr;
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

	class ReflectedClass
	{
	public:

		Ry::String Name;
		uint64_t Size;

		Ry::ArrayList<Ry::Field> Fields;
		Ry::ArrayList<Ry::Function> Functions;

		template<typename T>
		T* CreateInstance() const
		{
			return reinterpret_cast<T*>(CreateInstanceFunction());
		}

		void* (*CreateInstanceFunction)(void);		
	};

	// Singleton
	class CORE_MODULE ReflectionDatabase
	{
	public:

		void RegisterClass(const Ry::String& Name, const Ry::ReflectedClass* Class);
		const Ry::ReflectedClass* GetReflectedClass(const Ry::String& Name);
		const Ry::ReflectedClass* GetReflectedClass(Ry::String&& Name);

		Ry::OAPairIterator<Ry::String, const Ry::ReflectedClass*> GetClassIterator();
	private:


	};

	CORE_MODULE extern ReflectionDatabase RefDB;

	class ReflectionInitializer
	{
	public:
		ReflectionInitializer(Ry::String ClassName, const Ry::ReflectedClass* Class)
		{
			RefDB.RegisterClass(ClassName, Class);
		}
	};

	CORE_MODULE const Ry::ReflectedClass* GetReflectedClass(const Ry::String& Name);
	CORE_MODULE const Ry::ReflectedClass* GetReflectedClass(Ry::String&& Name);


}

// This implementation is generated for each reflected class/struct
template<typename T>
const Ry::ReflectedClass* GetClassImpl(Ry::TypeTag<T>)
{
	return nullptr;
}

template<typename T>
const Ry::ReflectedClass* GetClass()
{
	return GetClassImpl<T>();
}

template<typename T>
const Ry::DataType* GetTypeImpl(Ry::TypeTag<T>)
{
	static Ry::DataType Default{};
	return &Default;
}

template<typename T>
const Ry::DataType* GetType()
{
	return GetTypeImpl(Ry::TypeTag<T>{});
};

template<typename T>
const Ry::DataType* GetType(Ry::ArrayList<T> ArrayList)
{
	static Ry::DataType InnerType = GetType<T>();
	static Ry::DataType Result;

	Result.Size = sizeof(ArrayList);
	Result.Name = "Ry::ArrayList<" + InnerType.Name + ">";
	Result.Class = Ry::TypeClass::ArrayList;
	Result.TemplateType.Add(InnerType);

	return &Result;
};


REFLECT_PRIMITIVE(uint8, Ry::TypeClass::UInt8)
REFLECT_PRIMITIVE(uint16, Ry::TypeClass::UInt16)
REFLECT_PRIMITIVE(uint32, Ry::TypeClass::UInt32)
REFLECT_PRIMITIVE(uint64, Ry::TypeClass::UInt64)
REFLECT_PRIMITIVE(int8, Ry::TypeClass::Int8)
REFLECT_PRIMITIVE(int16, Ry::TypeClass::Int16)
REFLECT_PRIMITIVE(int32, Ry::TypeClass::Int32)
REFLECT_PRIMITIVE(int64, Ry::TypeClass::Int64)
REFLECT_PRIMITIVE(float, Ry::TypeClass::Float)
REFLECT_PRIMITIVE(double, Ry::TypeClass::Double)