#pragma once

#include "Object.gen.h"

#ifndef GeneratedBody
	#define GeneratedBody()
#endif

namespace Ry
{
	class ReflectedClass;

	class CORE_MODULE Object
	{
	public:
		virtual ~Object();
		virtual const ReflectedClass* GetClass();

		template<typename T>
		T& GetPropertyRef(Ry::String Name)
		{
			const ReflectedClass* Class = GetClass();

			const ReflectedField* FoundField = nullptr;

			for(const ReflectedField& Field : Class->Fields)
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
