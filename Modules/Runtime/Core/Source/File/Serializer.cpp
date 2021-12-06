#include "File/Serializer.h"

namespace Ry
{

	Serializer::Serializer()
	{
	}

	void Serializer::Open(const Ry::String& OutFile)
	{
		this->OutFile = OutFile;

		// Open file output stream
		this->Output.open(*OutFile, std::fstream::out | std::fstream::binary);
	}

	void Serializer::Close()
	{
		// Close file output stream
		this->Output.close();
	}

	void Serializer::WriteUByte(uint8 Byte)
	{
		Output << Byte;
	}

	void Serializer::WriteUShort(uint16 Short)
	{
		uint8 Byte0 = (Short) & 0xFF;
		uint8 Byte1 = (Short >> 8) & 0xFF;

		Output << Byte0;
		Output << Byte1;
	}

	void Serializer::WriteUInt(uint32 Int)
	{
		uint8 Byte0 = (Int) & 0xFF;
		uint8 Byte1 = (Int >> 8) & 0xFF;
		uint8 Byte2 = (Int >> 16) & 0xFF;
		uint8 Byte3 = (Int >> 24) & 0xFF;

		Output << Byte0;
		Output << Byte1;
		Output << Byte2;
		Output << Byte3;
	}

	void Serializer::WriteULongInt(uint64 LongInt)
	{
		uint8 Byte0 = (LongInt) & 0xFF;
		uint8 Byte1 = (LongInt >> 8) & 0xFF;
		uint8 Byte2 = (LongInt >> 16) & 0xFF;
		uint8 Byte3 = (LongInt >> 24) & 0xFF;
		uint8 Byte4 = (LongInt >> 32) & 0xFF;
		uint8 Byte5 = (LongInt >> 40) & 0xFF;
		uint8 Byte6 = (LongInt >> 48) & 0xFF;
		uint8 Byte7 = (LongInt >> 56) & 0xFF;

		Output << Byte0;
		Output << Byte1;
		Output << Byte2;
		Output << Byte3;
		Output << Byte4;
		Output << Byte5;
		Output << Byte6;
		Output << Byte7;
	}

	void Serializer::WriteByte(int8 Byte)
	{
		Output << Byte;
	}

	void Serializer::WriteShort(int16 Short)
	{
		uint8 Byte0 = (Short) & 0xFF;
		uint8 Byte1 = (Short >> 8) & 0xFF;

		Output << Byte0;
		Output << Byte1;
	}

	void Serializer::WriteInt(int32 Int)
	{
		uint8 Byte0 = (Int) & 0xFF;
		uint8 Byte1 = (Int >> 8) & 0xFF;
		uint8 Byte2 = (Int >> 16) & 0xFF;
		uint8 Byte3 = (Int >> 24) & 0xFF;

		char Bytes[] = {(char) Byte0, (char)Byte1, (char)Byte2, (char)Byte3};
		Output.write(Bytes, 4);
	}

	void Serializer::WriteLongInt(int64 LongInt)
	{
		uint8 Byte0 = (LongInt) & 0xFF;
		uint8 Byte1 = (LongInt >> 8) & 0xFF;
		uint8 Byte2 = (LongInt >> 16) & 0xFF;
		uint8 Byte3 = (LongInt >> 24) & 0xFF;
		uint8 Byte4 = (LongInt >> 32) & 0xFF;
		uint8 Byte5 = (LongInt >> 40) & 0xFF;
		uint8 Byte6 = (LongInt >> 48) & 0xFF;
		uint8 Byte7 = (LongInt >> 56) & 0xFF;

		Output << Byte0;
		Output << Byte1;
		Output << Byte2;
		Output << Byte3;
		Output << Byte4;
		Output << Byte5;
		Output << Byte6;
		Output << Byte7;
	}

	void Serializer::WriteString(const Ry::String& String)
	{
		WriteUInt(String.getSize());
		for(int32 Char = 0; Char < String.getSize(); Char++)
		{
			char Value = String[Char];
			WriteUByte(Value);
		}
	}

	void Serializer::WriteObject(const Ry::Object* Obj)
	{
		const ReflectedClass* Class = Obj->GetClass();

		// Start with the name of the class so we can resolve the root object type when the object is next read
		WriteString(Class->QualifiedName);

		// Write out how many fields there are
		WriteUInt(Class->Fields.GetSize());

		for (const Ry::Field& Field : Class->Fields)
		{
			SerializeField(Field, Obj);
		}
	}

	void Serializer::SerializeField(const Ry::Field& Field, const Ry::Object* Obj)
	{
		// Write out name of the field. You should be able to deduce the type of the field at runtime, so we don't store that.
		WriteString(Field.Name);

		if (Field.ObjectClass) // Child objects
		{
			SerializeObjectField(Field, Obj);
		}
		else if(Field.Type->Name == GetType<Ry::String>()->Name) // Strings
		{
			SerializeStringField(Field, Obj);
		}
		else if (Field.Type->Name == GetType<uint8>()->Name) // uint8
		{
			SerializeIntField<uint8>(Field, Obj);
		}
		else if (Field.Type->Name == GetType<uint16>()->Name) // uint16
		{
			SerializeIntField<uint16>(Field, Obj);
		}
		else if (Field.Type->Name == GetType<uint32>()->Name) // uint32
		{
			SerializeIntField<uint32>(Field, Obj);
		}
		else if (Field.Type->Name == GetType<uint64>()->Name) // uint64
		{
			SerializeIntField<uint64>(Field, Obj);
		}
		else if (Field.Type->Name == GetType<int8>()->Name) // int8
		{
			SerializeIntField<int8>(Field, Obj);
		}
		else if (Field.Type->Name == GetType<int16>()->Name) // int16
		{
			SerializeIntField<int16>(Field, Obj);
		}
		else if (Field.Type->Name == GetType<int32>()->Name) // int32
		{
			SerializeIntField<int32>(Field, Obj);
		}
		else if (Field.Type->Name == GetType<int64>()->Name) // int64
		{
			SerializeIntField<int64>(Field, Obj);
		}
	}

	void Serializer::SerializeObjectField(const Ry::Field& Field, const Ry::Object* Obj)
	{
		if(Ry::Object* const* ChildObject = Field.GetConstPtrToField<Ry::Object*>(Obj))
		{
			WriteObject(*ChildObject);
		}
	}

	void Serializer::SerializeStringField(const Ry::Field& Field, const Ry::Object* Obj)
	{
		if(const Ry::String* ChildString = Field.GetConstPtrToField<Ry::String>(Obj))
		{
			WriteString(*ChildString);
		}
	}


}
