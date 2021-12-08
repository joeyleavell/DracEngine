#pragma once

#include "Json/Json.h"
#include "Core/String.h"
#include "Reflection.h"

// Define this prior to including the gen.h in case these haven't been defined yet
#ifndef GeneratedBody
#define GeneratedBody()\
	static const ReflectedClass* GetStaticClass(){return nullptr;}; \
	static const DataType* GetStaticType(){return nullptr; };
#endif
#include "Object.gen.h"

namespace Ry
{
	class DataType;

	class CORE_MODULE ObjectBase
	{
	public:

		virtual const ReflectedClass* GetClass() const;

		static const DataType* GetStaticType()
		{
			static Ry::DataType Result;
			Result.Class = TypeClass::Object;
			Result.Name = "Ry::Object";
			Result.Size = sizeof(Ry::ObjectBase);
			return &Result; 
		};

		static const Ry::ReflectedClass* GetStaticClass()
		{
			static Ry::ReflectedClass C;
			C.Name = "Object";
			C.Size = sizeof(Ry::ObjectBase);
			C.CreateInstanceFunction = nullptr; // Should never create objects directly
			C.ParentClass = nullptr;
			C.Fields.SetSize(0);
			return &C;
		}

		template<typename T>
		T& GetPropertyRef(Ry::String Name)
		{
			const ReflectedClass* Class = GetClass();

			const Ry::Field* FoundField = nullptr;

			for(const Ry::Field& Field : Class->Fields)
			{
				if(Field.Name == Name)
				{
					FoundField = &Field;
				}
			}

			char* Mem = reinterpret_cast<char*>(reinterpret_cast<char*>(this) + FoundField->Offset);
			T* AsT = reinterpret_cast<T*>(Mem);

			return *AsT;
		}

	protected:

		ObjectBase() = default;
		virtual ~ObjectBase() = default;
	};

	class CORE_MODULE Object : public ObjectBase
	{
	public:
		GeneratedBody()

		void SetObjectName(const Ry::String& NewName);
		Ry::String GetObjectName() const;

	protected:

		RefField()
		Ry::String ObjectName;

	} RefClass();

	template<typename T>
	EXPORT_ONLY T* NewObject(Ry::String ObjectName)
	{
		T* NewObject = new T;
		NewObject->SetObjectName(ObjectName);

		return NewObject;
	}

	CORE_MODULE Json Jsonify(Ry::Object& Object);
	
}
