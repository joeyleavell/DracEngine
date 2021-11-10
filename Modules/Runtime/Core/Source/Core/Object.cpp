#include "Core/Object.h"

namespace Ry
{

	Object::~Object()
	{
		
	}

	const ReflectedClass* Object::GetClass()
	{
		return nullptr;
	}

	Json Jsonify(Ry::Object& Object)
	{
		Json RootJson = Json::CreateObject();
		const ReflectedClass* Class = Object.GetClass();

		for(const Ry::Field& RefField : Class->Fields)
		{
			// Unsigned integral types
			if (RefField.Type->Class == TypeClass::UInt8)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<uint8>(&Object));
			if (RefField.Type->Class == TypeClass::UInt16)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<uint16>(&Object));
			if (RefField.Type->Class == TypeClass::UInt32)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<uint32>(&Object));
			if (RefField.Type->Class == TypeClass::UInt64)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<uint64>(&Object));

			// Signed integral types
			if (RefField.Type->Class == TypeClass::Int8)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<int8>(&Object));
			if (RefField.Type->Class == TypeClass::Int16)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<int16>(&Object));
			if (RefField.Type->Class == TypeClass::Int32)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<int32>(&Object));
			if (RefField.Type->Class == TypeClass::Int64)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<int64>(&Object));

			// Floating point types
			if (RefField.Type->Class == TypeClass::Float)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<float>(&Object));
			if (RefField.Type->Class == TypeClass::Double)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<double>(&Object));

			// Strings
			if (RefField.Type->Class == TypeClass::String)
				RootJson.Insert(RefField.Name, *RefField.GetPtrToField<Ry::String>(&Object));
		}

		return RootJson;
	}
}
