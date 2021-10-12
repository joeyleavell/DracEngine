#pragma once

#include "Core/Core.h"
#include "Object.gen.h"

#ifndef GeneratedBody
	#define GeneratedBody()\
	static const ReflectedClass* GetStaticClass(){return nullptr;}; \
	static const DataType* GetStaticType(){return nullptr; };
#endif

namespace Ry
{
	class ReflectedClass;

	class CORE_MODULE Object
	{
	public:
		virtual ~Object();
		virtual const ReflectedClass* GetClass();

		static const Ry::ReflectedClass* GetStaticClass()
		{
			static Ry::ReflectedClass C;
			C.Name = "Object";
			C.Size = sizeof(Ry::Object);
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
	};

}
