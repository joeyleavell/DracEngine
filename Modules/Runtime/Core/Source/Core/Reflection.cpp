#include "Core/Reflection.h"
#include "Data/Map.h"

namespace Ry
{
	static Ry::OAHashMap<Ry::String, const Ry::ReflectedClass*> ReflectedClasses;
	
	ReflectionDatabase RefDB;

	const Ry::Field* ReflectedClass::FindFieldByName(const Ry::String& Name) const
	{
		for(const Field& Field : Fields)
		{
			if (Field.Name == Name)
				return &Field;
		}
		
		return nullptr;
	}

	void ReflectionDatabase::RegisterClass(const Ry::String& Name, const Ry::ReflectedClass* Class)
	{
		ReflectedClasses.Insert(Name, Class);
	}

	const Ry::ReflectedClass* ReflectionDatabase::GetReflectedClass(const Ry::String& Name)
	{
		return ReflectedClasses.Get(Name);
	}

	const Ry::ReflectedClass* ReflectionDatabase::GetReflectedClass(Ry::String&& Name)
	{
		return ReflectedClasses.Get(Name);
	}

	Ry::OAPairIterator<Ry::String, const Ry::ReflectedClass*> ReflectionDatabase::GetClassIterator()
	{
		return ReflectedClasses.CreatePairIterator();
	}

	const Ry::ReflectedClass* GetReflectedClass(const Ry::String& Name)
	{
		return RefDB.GetReflectedClass(Name);
	}

	const Ry::ReflectedClass* GetReflectedClass(Ry::String&& Name)
	{
		return RefDB.GetReflectedClass(Name);
	}
	
}
