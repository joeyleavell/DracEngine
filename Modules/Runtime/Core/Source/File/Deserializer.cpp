#include "File/Deserializer.h"
#include "Core/Globals.h"

namespace Ry
{
	Deserializer::Deserializer()
	{

	}

	void Deserializer::Open(const Ry::String& InputFile)
	{
		this->InputFile = InputFile;

		Input.open(*InputFile, std::fstream::binary | std::fstream::in);
	}

	void Deserializer::Close()
	{
		Input.close();
	}

	uint8 Deserializer::ReadUByte()
	{
		char Num;
		Input.read(&Num, 1);

		return Num;
	}

	uint16 Deserializer::ReadUShort()
	{
		char Num[2];
		Input.read(Num, 2);

		uint16 Result = (Num[0] >> 0) | (Num[1] >> 8);
		return Result;
	}

	uint32 Deserializer::ReadUInt()
	{
		char Num[4];
		Input.read(Num, 4);

		uint16 Result = (Num[0] >> 0) | (Num[1] >> 8) | (Num[2] >> 16) | (Num[3] >> 24);
		return Result;
	}

	uint64 Deserializer::ReadULongInt()
	{
		char Num[8];
		Input.read(Num, 8);

		uint8* AsUInt64 = reinterpret_cast<uint8*>(Num);
		
		uint64 Result = ((uint64)AsUInt64[0] << 0L) |
			((uint64)AsUInt64[1] << 8L) |
			((uint64)AsUInt64[2] << 16L) |
			((uint64)AsUInt64[3] << 24L) |
			((uint64)AsUInt64[4] << 32L) |
			((uint64)AsUInt64[5] << 40L) |
			((uint64)AsUInt64[6] << 48L) |
			((uint64)AsUInt64[7] << 56L);

		return Result;
	}

	int8 Deserializer::ReadByte()
	{
		char Num;
		Input.read(&Num, 1);

		return Num;
	}

	int16 Deserializer::ReadShort()
	{
		char Num[2];
		Input.read(Num, 2);

		int16 Result = (Num[0] >> 0) | (Num[1] >> 8);
		return Result;
	}

	int32 Deserializer::ReadInt()
	{
		char Num[4];
		Input.read(Num, 4);

		int16 Result = (Num[0] >> 0) | (Num[1] >> 8) | (Num[2] >> 16) | (Num[3] >> 24);
		return Result;
	}

	int64 Deserializer::ReadLongInt()
	{
		char Num[8];
		Input.read(Num, 8);

		uint8* AsUInt64 = reinterpret_cast<uint8*>(Num);

		int64 Result = ((uint64)AsUInt64[0] << 0L) |
			((uint64)AsUInt64[1] << 8L) |
			((uint64)AsUInt64[2] << 16L) |
			((uint64)AsUInt64[3] << 24L) |
			((uint64)AsUInt64[4] << 32L) |
			((uint64)AsUInt64[5] << 40L) |
			((uint64)AsUInt64[6] << 48L) |
			((uint64)AsUInt64[7] << 56L);

		return Result;
	}

	Ry::String Deserializer::ReadString()
	{
		uint32 Size = ReadUInt();
		Ry::String Result(Size);

		for(uint32 Index = 0; Index < Size; Index++)
		{
			char Next = ReadUByte();
			Result[Index] = Next;
		}

		// Append the null terminator
		Result[Size] = '\0';

		return Result;
	}

	Ry::Object* Deserializer::ReadObject()
	{
		Ry::String ClassName = ReadString();

		const Ry::ReflectedClass* ReflectedClass = GetReflectedClass(ClassName);

		if(ReflectedClass)
		{
			// Create a new instance of the class
			Ry::Object* NewInstance = ReflectedClass->CreateInstance<Ry::Object>();

			if(NewInstance)
			{
				// Get how many fields
				uint32 FieldCount = ReadUInt();

				// Iterate through all reflected fields and populate the class's data
				for (int32 FieldIndex = 0; FieldIndex < FieldCount; FieldIndex++)
				{
					// Read field name and size
					Ry::String FieldName = ReadString();
					uint64 FieldSize = ReadULongInt();

					if (const Field* FoundField = ReflectedClass->FindFieldByName(FieldName))
					{
						DeserializeField(FoundField, NewInstance, FieldSize);
					}
					else
					{
						// Todo: how do we handle this? Probably just ignore it since this could happen when an asset's data gets updated
						Ry::Log->LogErrorf("Deserializer::ReadObject: Field %s that was serialized in %s was not found in reflected class %s. Has the class's data changed?", *FieldName, *InputFile, *ClassName);

						IgnoreBytes(FieldSize);
					}

				}
			}
			else
			{
				Ry::Log->LogErrorf("Deserializer::ReadObject: Failed to create new instance of object %s when reading object", *ClassName);
			}

			return NewInstance;
		}
		else
		{
			Ry::Log->LogErrorf("Deserializer::ReadObject: Failed to get reflected class of type %s", *ClassName);
			return nullptr;
		}

	}

	void Deserializer::IgnoreBytes(uint64 Count)
	{
		// Skip over the rest of this field's bytes since we won't be able to extract this field
		// Because ignore() only takes in an int32, this ensures we can also ignore int64's, although very unlikely.
		uint64 ToIgnore = Count;
		while (ToIgnore > 0)
		{
			const uint32 IgnoreAmount = (uint32)std::min(ToIgnore, (uint64)INT32_MAX);
			Input.ignore(IgnoreAmount);

			ToIgnore -= IgnoreAmount;
		}
	}

	void Deserializer::DeserializeField(const Field* Field, Ry::Object* DstObject, uint64 FieldSize)
	{
		if(Field->ObjectClass) // Child object
		{
			DeserializeField_Helper<Ry::Object*>(Field, DstObject, &Deserializer::ReadObject);
		}
		else if(Field->Type->Class == TypeClass::ArrayList)
		{
			DeserializeArrayListField(Field, DstObject);
		}
		else if(Field->Type->Name == GetType<Ry::String>()->Name) // Strings
		{
			DeserializeField_Helper<Ry::String>(Field, DstObject, &Deserializer::ReadString);
		}
		else if (Field->Type->Name == GetType<uint8>()->Name) // uint8
		{
			DeserializeField_Helper<uint8>(Field, DstObject, &Deserializer::ReadUByte);
		}
		else if (Field->Type->Name == GetType<uint16>()->Name) // uint16
		{
			DeserializeField_Helper<uint16>(Field, DstObject, &Deserializer::ReadUShort);
		}
		else if (Field->Type->Name == GetType<uint32>()->Name) // uint32
		{
			DeserializeField_Helper<uint32>(Field, DstObject, &Deserializer::ReadUInt);
		}
		else if (Field->Type->Name == GetType<uint64>()->Name) // uint64
		{
			DeserializeField_Helper<uint64>(Field, DstObject, &Deserializer::ReadULongInt);
		}
		else if (Field->Type->Name == GetType<int8>()->Name) // int8
		{
			DeserializeField_Helper<int8>(Field, DstObject, &Deserializer::ReadByte);
		}
		else if (Field->Type->Name == GetType<int16>()->Name) // int16
		{
			DeserializeField_Helper<int16>(Field, DstObject, &Deserializer::ReadShort);
		}
		else if (Field->Type->Name == GetType<int32>()->Name) // int32
		{
			DeserializeField_Helper<int32>(Field, DstObject, &Deserializer::ReadInt);
		}
		else if (Field->Type->Name == GetType<int64>()->Name) // int64
		{
			DeserializeField_Helper<int64>(Field, DstObject, &Deserializer::ReadLongInt);
		}
		else
		{
			Ry::Log->LogErrorf("Unsupported data type %s, skipping", *Field->Type->Name);

			// Skip bytes
			IgnoreBytes(FieldSize);
		}
	}

	void Deserializer::DeserializeArrayListField(const Field* Field, Ry::Object* DstObject)
	{
		Ry::String ElementType = ReadString();

		// Check if this is an object type
		if(GetReflectedClass(ElementType) || ElementType == "Ry::Object")
		{
			DeserializeArrayListField_Helper<Ry::Object*>(Field, DstObject, &Deserializer::ReadObject);
		}
		else if(ElementType == GetType<Ry::String>()->Name)
		{
			DeserializeArrayListField_Helper<Ry::String>(Field, DstObject, &Deserializer::ReadString);
		}
		else if (ElementType == GetType<uint8>()->Name)
		{
			DeserializeArrayListField_Helper<uint8>(Field, DstObject, &Deserializer::ReadUByte);
		}
		else if (ElementType == GetType<uint16>()->Name)
		{
			DeserializeArrayListField_Helper<uint16>(Field, DstObject, &Deserializer::ReadUShort);
		}
		else if (ElementType == GetType<uint32>()->Name)
		{
			DeserializeArrayListField_Helper<uint32>(Field, DstObject, &Deserializer::ReadUInt);
		}
		else if (ElementType == GetType<uint64>()->Name)
		{
			DeserializeArrayListField_Helper<uint64>(Field, DstObject, &Deserializer::ReadULongInt);
		}
		else if (ElementType == GetType<int8>()->Name)
		{
			DeserializeArrayListField_Helper<int8>(Field, DstObject, &Deserializer::ReadByte);
		}
		else if (ElementType == GetType<int16>()->Name)
		{
			DeserializeArrayListField_Helper<int16>(Field, DstObject, &Deserializer::ReadShort);
		}
		else if (ElementType == GetType<int32>()->Name)
		{
			DeserializeArrayListField_Helper<int32>(Field, DstObject, &Deserializer::ReadInt);
		}
		else if (ElementType == GetType<int64>()->Name)
		{
			DeserializeArrayListField_Helper<int64>(Field, DstObject, &Deserializer::ReadLongInt);
		}
	}

}
