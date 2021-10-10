#include "Core/Reflection.h"
#include "Data/Map.h"

namespace Ry
{
	static Ry::OAHashMap<Ry::String, const Ry::ReflectedClass*> ReflectedClasses;
	
	ReflectionDatabase RefDB;

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
	
}
