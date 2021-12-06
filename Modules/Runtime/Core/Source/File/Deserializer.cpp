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

		uint64 Result = (Num[0] >> 0) | (Num[1] >> 8) | (Num[2] >> 16) | (Num[3] >> 24) | (Num[4] >> 0) | (Num[5] >> 8) | (Num[6] >> 16) | (Num[7] >> 24);
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

		int64 Result = (Num[0] >> 0) | (Num[1] >> 8) | (Num[2] >> 16) | (Num[3] >> 24) | (Num[4] >> 0) | (Num[5] >> 8) | (Num[6] >> 16) | (Num[7] >> 24);
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
						DeserializeField(FoundField, NewInstance);
					}
					else
					{
						// Todo: how do we handle this? Probably just ignore it since this could happen when an asset's data gets updated
						Ry::Log->LogErrorf("Deserializer::ReadObject: Field that was serialized in %s was not found in reflected class %s. Has the class's data changed?", *InputFile, *ClassName);

						// Skip over the rest of this field's bytes since we won't be able to extract this field
						// Because ignore() only takes in an int32, this ensures we can also ignore int64's, although very unlikely.
						uint64 ToIgnore = FieldSize;
						while(ToIgnore > 0)
						{
							const uint32 IgnoreAmount = (uint32)std::min(FieldSize, (uint64)INT32_MAX);
							Input.ignore(IgnoreAmount);
							
							ToIgnore -= IgnoreAmount;
						}
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

	void Deserializer::DeserializeField(const Field* Field, Ry::Object* DstObject)
	{
		if(Field->ObjectClass) // Child object
		{
			DeserializeField_Helper<Ry::Object*>(Field, DstObject, &Deserializer::ReadObject);
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
	}

}
